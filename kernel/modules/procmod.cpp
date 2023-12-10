#include <ng/fs.h>
#include <ng/mod.h>

void module_procfile(file *ofd, void *)
{
    proc_sprintf(ofd, "Hello World from a kernel module\n");
}

int modinit(mod *)
{
    make_proc_file("mod", module_procfile, nullptr);
    return MODINIT_SUCCESS;
}

modinfo modinfo = {
    .name = "procmod",
    .init = modinit,
};
