
#include <ng/basic.h>
#include <ng/elf.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ds/vector.h>
#include <ds/dmgr.h>

Elf *ngk_elf_header = (Elf *)0xFFFFFFFF80000000;

int load_module(Elf *elf, size_t len) {
        if (!elf_verify(elf))
                return EINVAL;

        size_t init_offset = elf_get_sym_off("init_module", elf);
        if (init_offset == 0) {
                return ENOEXEC;
        }

        Elf *loaded_elf = malloc(len);
        memcpy(loaded_elf, elf, len);

        elf_resolve_symbols_from_elf(ngk_elf_header, loaded_elf);
        elf_relocate_object(loaded_elf, (uintptr_t)loaded_elf);

        // Question: should I use -Wno-(whatever the warning is)
        // to stop the "ISO C does not allow ptr->fptr" problem?
        void *init_module_v = elf_at(loaded_elf, init_offset);
        init_module_t *init_module = *(init_module_t **)&init_module_v;

        init_module();

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
                RETURN_ERROR(EBADF);
        }

        if (node->filetype != MEMORY_BUFFER) {
                RETURN_ERROR(EPERM);
        }

        int err = load_module((Elf *)node->extra.memory, node->len);

        if (err != 0)
                RETURN_ERROR(err);
        RETURN_VALUE(0);
}

