
#include <ng/basic.h>
#include <ng/elf.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ds/dmgr.h>
#include <ds/list.h>
#include <ds/vector.h>

struct list loaded_mods = {0};

void init_mods() {

}

int load_mod(Elf *elf, size_t len) {
        if (!elf_verify(elf))
                return EINVAL;

        // high_vmm_reserve is the only way I have to get memory above -2G on
        // X64 right now - this should prooooobably be replaced by a dedicated
        // malloc pool?
        //
        // Right now it doesn't matter because I don't support unloading
        // modules, so releasing the memory isn't a thing I need to worry about
        Elf *loaded_elf = high_vmm_reserve(len);
        memcpy(loaded_elf, elf, len);

        struct elfinfo ei = elf_info(loaded_elf);

        size_t init_offset = elf_get_sym_off(&ei, "init_mod");
        if (init_offset == 0) {
                return ENOEXEC;
        }

        elf_resolve_symbols(&ngk_elfinfo, &ei);
        elf_relocate_object(&ei, (uintptr_t)loaded_elf);

        // Question: should I use -Wno-(whatever the warning is)
        // to stop the "ISO C does not allow ptr->fptr" problem?

        void *init_mod_v = elf_at(loaded_elf, init_offset);
        init_mod_t *init_mod = *(init_mod_t **)&init_mod_v;

        init_mod(0);

        return 0;
}

struct syscall_ret sys_loadmod(int fd) {
        struct vector *fds = &running_process->fds;
        if (fd > fds->len) {
                RETURN_ERROR(EBADF);
        }

        int file_handle = vec_get_value(fds, fd);
        struct fs_node *node = dmgr_get(&fs_node_table, file_handle);
        if (!node) {
                RETURN_ERROR(EBADF); }

        if (node->filetype != MEMORY_BUFFER) {
                RETURN_ERROR(EPERM);
        }

        int err = load_mod((Elf *)node->extra.memory, node->len);

        if (err != 0)
                RETURN_ERROR(err);
        RETURN_VALUE(0);
}

