
#include <basic.h>
#include <ng/mod.h>
#include <ng/fs.h>
#include <ng/procfile.h>
#include <nc/stdio.h>
#include <nc/stdlib.h>

void module_procfile(struct open_file *ofd) {
        ofd->buffer = malloc(1024);
        ofd->length = sprintf(
                        ofd->buffer,
                        "Hello World from a kernel module\n");
}

enum modinit_status modinit(struct mod *_) {
        make_procfile("mod", module_procfile, NULL);
        return MODINIT_SUCCESS;
}

struct modinfo modinfo = {
        .name = "procfile module",
        .modinit = modinit,
};

