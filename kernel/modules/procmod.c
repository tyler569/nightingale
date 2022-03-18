#include <basic.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>
#include <stdlib.h>

void module_procfile(struct file *ofd, void *_)
{
    proc2_sprintf(ofd, "Hello World from a kernel module\n");
}

int modinit(struct mod *_)
{
    make_proc_file2("mod", module_procfile, NULL);
    return MODINIT_SUCCESS;
}

struct modinfo modinfo = {
    .name = "procmod",
    .init = modinit,
};
