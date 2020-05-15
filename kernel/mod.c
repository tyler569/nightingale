
#include <basic.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/dmgr.h>
#include <linker/elf.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>

struct list loaded_mods = {0};

/*
void init_mods() {

}
*/

int load_mod(Elf *elf, size_t len) {
        if (!elf_verify(elf))
                return -EINVAL;

        // high_vmm_reserve is the only way I have to get memory above -2G on
        // X64 right now - this should prooooobably be replaced by a dedicated
        // malloc pool?
        //
        // Right now it doesn't matter because I don't support unloading
        // modules, so releasing the memory isn't a thing I need to worry about
        Elf *loaded_elf = high_vmm_reserve(len);
        memcpy(loaded_elf, elf, len);

        struct elfinfo ei = elf_info(loaded_elf);

        size_t init_offset = elf_get_sym_off(&ei, "modinfo");
        if (init_offset == 0) {
                return -ENOEXEC;
        }

        elf_resolve_symbols(&ngk_elfinfo, &ei);
        elf_relocate_object(&ei, (uintptr_t)loaded_elf);

        struct modinfo *modinfo = elf_at(loaded_elf, init_offset);
        printf("loaded mod \"%s\" to %p, calling init\n", 
                        modinfo->name, loaded_elf);
        if (!modinfo->modinit) {
                return -ENOEXEC; // what do?
        }
        enum modinit_status status = modinfo->modinit(NULL);
        if (status != MODINIT_SUCCESS) {
                return -ETODO; // what do?
        }

        return 0;
}

sysret sys_loadmod(int fd) {
        struct dmgr *fds = &running_process->fds;

        struct open_file *ofd = dmgr_get(fds, fd);
        if (ofd == NULL) {
                return -EBADF;
        }

        struct file *node = ofd->node;
        if (node->filetype != FT_BUFFER) {
                return -EPERM;
        }

        return load_mod((Elf *)node->memory, node->len);
}

