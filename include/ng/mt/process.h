#pragma once
#ifndef NG_MT_PROCESS_H
#define NG_MT_PROCESS_H

#include "ng/memmap.h"
#include "thread.h"
#include <ng/thread.h>
#include <nx/list.h>
#include <nx/vector.h>

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
    struct dentry *root { nullptr };

    nx::list_node siblings {};
    nx::list_node trace_node {};
    nx::list<process, &process::siblings> children {};
    nx::list<thread, &thread::process_threads> threads {};

    uintptr_t mmap_base { USER_MMAP_BASE };
    struct mm_region mm_regions[NREGIONS] {};

    elf_md *elf_metadata {};
};

#endif // NG_MT_PROCESS_H
