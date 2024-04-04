#pragma once

#include "ng/memmap.h"
#include "ng/mman.h"
#include "thread.h"
#include <ng/thread.h>
#include <nx/list.h>
#include <nx/vector.h>

namespace fs3 {
class open_file;
}

constexpr int proc_magic = 0x706f7273; // "proc"
extern char boot_pt_root;

struct process {
    pid_t pid { 0 };
    pid_t pgid { pid };
    char comm[COMM_SIZE] {};

    unsigned int magic { proc_magic };

    phys_addr_t vm_root {};

    int uid {};
    int gid {};

    int exit_intention {}; // tells threads to exit
    int exit_status {}; // tells parent has exited

    struct process *parent { nullptr };

    nx::vector<file *> files {};
    nx::vector<mem_region> mem_regions {};
    struct dentry *root { nullptr };

    nx::list_node siblings {};
    nx::list_node trace_node {};
    nx::list<process, &process::siblings> children {};
    nx::list<thread, &thread::process_threads> threads {};

    uintptr_t mmap_base { USER_MMAP_BASE };

    elf_md *elf_metadata {};

    nx::vector<fs3::open_file *> m_fd3s;

    virt_addr_t allocate_mmap_space(size_t size);

    mem_region *find_mem_region(uintptr_t addr);
    void add_unbacked_mem_region(uintptr_t base, size_t size);
    void add_unbacked_mem_region(size_t size);
    void add_file_mem_region(uintptr_t base, size_t size, file *f, off_t offset,
        int prot, int flags);
};

