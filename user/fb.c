#include <errno.h>
#include <fcntl.h>
#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    struct framebuffer_info info;
    int fd = open("/dev/fb", O_RDWR);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, FB_IOCTL_GET_INFO, &info) < 0) {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    printf("width: %d\n", info.width);
    printf("height: %d\n", info.height);
    printf("bpp: %d\n", info.bpp);
    printf("pitch: %d\n", info.pitch);

    void *fb
        = mmap(NULL, info.height * info.pitch, PROT_WRITE, MAP_SHARED, fd, 0);
    uint32_t *fb32 = (uint32_t *)fb;

    for (int i = 0; i < info.height; i++) {
        for (int j = 0; j < info.width; j++) {
            uint32_t color = 0xFF000000;
            if (i < info.height / 2) {
                color = 0xFFFF0000;
            }
            if (j < info.width / 2) {
                color = 0xFF00FF00;
            }
            if (i < info.height / 2 && j < info.width / 2) {
                color = 0xFF0000FF;
            }
            fb32[i * info.width + j] = color;
        }
    }

    return 0;
}