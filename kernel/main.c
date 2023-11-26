#include <basic.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/debug.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/random.h>
#include <ng/serial.h>
#include <ng/tarfs.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <elf.h>
#include <version.h>
#include <x86/acpi.h>
#include <x86/apic.h>
#include <x86/cpu.h>
#include <x86/interrupt.h>
#include <x86/pic.h>
#include "fs/init.h"
#include "limine.h"
#include "proc_files.h"

struct tar_header *initfs;
int have_fsgsbase = 0;
bool initialized = false;

const char *banner = "\n\
********************************\n\
\n\
The Nightingale Operating System\n\
Version " NIGHTINGALE_VERSION "\n\
\n\
********************************\n\
\n\
Copyright (C) 2017-2023, Tyler Philbrick\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n";

bool print_boot_info = true;

extern struct thread thread_zero;

void real_main(void);
extern char hhstack_top;

// move me
void ext2_info(void);

// move me
void tty_init(void)
{
    struct tty *new_tty(struct serial_device * dev, int id);
    extern struct serial_device *x86_com[2];
    new_tty(x86_com[0], 0);
    new_tty(x86_com[1], 1);
}

void early_init(void)
{
    set_gs_base(thread_cpus[0]);
    idt_install();

    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();

    arch_init();
    acpi_init(limine_rsdp());

    tty_init();
    pm_init();
    limine_init();
}

uint64_t tsc;

__USED
noreturn void kernel_main(void)
{
    tsc = rdtsc();

    early_init();

    random_dance();
    event_log_init();
    timer_init();
    longjump_kcode((uintptr_t)real_main, (uintptr_t)&hhstack_top);
}

#define TEXT_SCALE 1

uint32_t *fb;
uint32_t width, height;
void video_print(uint32_t x, uint32_t y, const char *str)
{
    extern unsigned char font8x8_basic[128][8];

    for (size_t i = 0; i < strlen(str); i++) {
        for (size_t j = 0; j < 8 * TEXT_SCALE; j++) {
            for (size_t k = 0; k < 8 * TEXT_SCALE; k++) {
                if (font8x8_basic[(int)str[i]][j / TEXT_SCALE]
                    & (1 << (k / TEXT_SCALE)))
                    fb[(y + j) * width + i * 8 * TEXT_SCALE + x + k]
                        = 0xffffffff;
            }
        }
    }
}

void video_scroll(uint32_t lines)
{
    lines *= 8 * TEXT_SCALE;
    for (size_t i = 0; i < height - lines; i++) {
        memcpy(fb + i * width, fb + (i + lines) * width, width * 4);
    }
    for (size_t i = height - lines; i < height; i++) {
        memset(fb + i * width, 0, width * 4);
    }
}

void real_main(void)
{
    printf("real_main\n");

    void *kernel_file_ptr = limine_kernel_file_ptr();
    size_t kernel_file_len = limine_kernel_file_len();
    limine_load_kernel_elf(kernel_file_ptr, kernel_file_len);

    initfs = limine_module();
    fs_init(initfs);
    threads_init();

    if (print_boot_info)
        pci_enumerate_bus_and_print();

    procfs_init();
    run_all_tests();

    initialized = true;

    const char *init_program = get_kernel_argument("init");
    if (!init_program)
        init_program = "/bin/init";
    bootstrap_usermode(init_program);

    // ext2_info();

    printf("%s", banner);

    if (print_boot_info) {
        printf("threads: usermode thread installed\n");
        printf("initialization took: %li\n", rdtsc() - tsc);
        printf("cpu: allowing irqs\n");
    }

    {
        uint32_t pitch, bpp;
        void *address;
        limine_framebuffer(&width, &height, &bpp, &pitch, &address);

        printf("framebuffer: %ux%u, %u bpp, pitch %u, address %p\n", width,
            height, bpp, pitch, address);

        if (bpp != 32)
            panic("framebuffer bpp is not 32");

        fb = address;

        video_print(0, 0, "Hello world!");
        video_print(0, 16, "abcdefghijklmnopqrstuvwxyz");
        video_print(0, 32, "0123456789");
        video_print(0, 48, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        video_print(0, 64, "`~!@#$%^&*(){}[]_+-=:;\"'<>,.?/\\");
        video_print(0, 80, "video_print(128, 128, \"Hello world!\");");

        int y = 96;

        char buf[64];
        int i = 1;
        while (i++) {
            sprintf(buf, "Hello world! %i", i);
            video_print(0, y, buf);
            y += 8 * TEXT_SCALE;
            if (y >= height) {
                y -= 8 * TEXT_SCALE;
                video_scroll(1);
            }
        }
    }

    enable_irqs();

    void ap_kernel_main();
    limine_smp_init(1, ap_kernel_main);

    while (true)
        __asm__ volatile("hlt");
    panic("kernel_main tried to return!");
}

void ap_kernel_main(void)
{
    printf("\nthis is the application processor\n");
    arch_ap_init();
    printf("lapic: initialized\n");
}
