#include <elf.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/common.h>
#include <ng/debug.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/fs/init.h>
#include <ng/limine.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/proc_files.h>
#include <ng/random.h>
#include <ng/serial.h>
#include <ng/tarfs.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/tty.h>
#include <ng/vmm.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/pic.h>
#include <nx/list.h>
#include <nx/string.h>
#include <nx/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <version.h>

// where is this coming from in C++ mode?
#undef noreturn

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

extern thread thread_zero;

[[noreturn]] void real_main();
extern char hhstack_top;

// move me
extern "C" void ext2_info(void);

extern "C" void early_init(void)
{
    set_gs_base(thread_cpus[0]);
    idt_install();

    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();

    arch_init();
    acpi_init(reinterpret_cast<acpi_rsdp_t *>(limine_rsdp()));

    tty_init();
    pm_init();
    limine_init();
}

uint64_t tsc;

__USED
extern "C" [[noreturn]] void kernel_main(void)
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

[[noreturn]] void real_main()
{
    printf("real_main\n");

    void *kernel_file_ptr = limine_kernel_file_ptr();
    size_t kernel_file_len = limine_kernel_file_len();
    limine_load_kernel_elf(kernel_file_ptr, kernel_file_len);

    initfs = (tar_header *)limine_module();
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

        fb = (uint32_t *)address;

        video_print(0, 0, "Hello world!");
        video_print(0, 16, "abcdefghijklmnopqrstuvwxyz");
        video_print(0, 32, "0123456789");
        video_print(0, 48, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        video_print(0, 64, "`~!@#$%^&*(){}[]_+-=:;\"'<>,.?/\\");
        video_print(0, 80, "video_print(128, 128, \"Hello world!\");");

        int y = 96;

        char buf[64];
        for (int i = 0; i < 30; i++) {
            sprintf(buf, "Hello world! %i", i);
            video_print(0, y, buf);
            y += 8 * TEXT_SCALE;
            if (y >= height) {
                y -= 8 * TEXT_SCALE;
                video_scroll(1);
            }
        }
    }

    void cpp_test();
    cpp_test();

    enable_irqs();

    void ap_kernel_main();
    limine_smp_init(1, reinterpret_cast<limine_goto_address>(ap_kernel_main));

    while (true)
        __asm__ volatile("hlt");
    panic("kernel_main tried to return!");
}

void ap_kernel_main()
{
    printf("\nthis is the application processor\n");
    arch_ap_init();
    printf("lapic: initialized\n");
}

void cpp_test()
{
    nx::string str = "Hello world!";
    printf("str: %s\n", str.c_str());
    nx::string_view sv = str;
    printf("sv: %s\n", sv.c_str());
    nx::vector<int> vec;
    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }
    printf("vec: ");
    for (auto &i : vec) {
        printf("%i ", i);
    }
    printf("\n");

    class foo {
    public:
        int m_value;
        nx::list_node link {};

        explicit constexpr foo(int value)
            : m_value(value)
        {
        }
    };

    nx::list<foo, &foo::link> lst;
    foo f1 { 1 }, f2 { 2 }, f3 { 3 };
    lst.push_back(f1);
    lst.push_back(f2);
    lst.push_back(f3);
    printf("lst: ");
    for (auto &i : lst) {
        printf("%i ", i.m_value);
    }
    printf("\n");
}