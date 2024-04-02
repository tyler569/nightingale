#include <ng/common.h>
#include <ng/fs.h>
#include <ng/fs/fb.h>
#include <ng/limine.h>
#include <ng/mt/process.h>
#include <ng/vmm.h>
#include <nightingale.h>
#include <sys/mman.h>

void *framebuffer_mmap(
    struct file *file, void *addr, size_t len, int prot, int flags, off_t off)
{
    uint32_t width, height, bpp, pitch;
    void *address;

    limine_framebuffer(&width, &height, &bpp, &pitch, &address);
    phys_addr_t phys
        = ROUND_DOWN(vmm_virt_to_phy((uintptr_t)address), PAGE_SIZE);

    size_t real_len = MIN(len, pitch * height);

    assert(off == 0);
    assert(addr == NULL);
    assert(prot == MAP_PRIVATE);
    assert(flags == MAP_SHARED);

    real_len = ROUND_UP(real_len, PAGE_SIZE);

    uintptr_t usable = running_process->mmap_base;
    running_process->mmap_base += real_len;

    user_map(usable, usable + real_len);
    vmm_map_range(usable, phys, real_len, PAGE_WRITEABLE | PAGE_USERMODE);

    return (void *)usable;
}

int framebuffer_ioctl(struct file *file, int request, void *argp)
{
    switch (request) {
    case FB_IOCTL_GET_INFO: {
        uint32_t width, height, bpp, pitch;
        void *address;
        limine_framebuffer(&width, &height, &bpp, &pitch, &address);
        struct framebuffer_info info = {
            .width = width,
            .height = height,
            .bpp = bpp,
            .pitch = pitch,
        };
        memcpy(argp, &info, sizeof(info));
        return 0;
    }
    default:
        return -EINVAL;
    }
}

struct file_operations framebuffer_ops = {
    .ioctl = framebuffer_ioctl,
    .mmap = framebuffer_mmap,
};