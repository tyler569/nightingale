#pragma once

#include <elf.h>
#include <ng/fs.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/trace.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

#define COMM_SIZE 32

struct thread;
struct process;

struct cpu {
    struct cpu *self;

    struct thread *idle;
    struct thread *running;
};

void new_cpu(int n);

#define NCPUS 32
extern struct cpu *thread_cpus[NCPUS];

#define this_cpu ((void)0, (struct cpu __seg_gs *)0)
// #define this_addr ((void)0, this_cpu->self)

// on x86, the floating point context for a process is an opaque
// 512 byte region.  This is probably not super portable;
typedef char fp_ctx[512] __ALIGN(16);

enum mm_flags {
    MM_FILE = (1 << 1),
    MM_SHARED = (1 << 2),
};

struct mm_region {
    uintptr_t base;
    uintptr_t top;
    enum mm_flags flags;
    struct inode *inode;
};
#define NREGIONS 32

enum thread_state {
    TS_INVALID,
    TS_PREINIT, // allocated, not initialized
    TS_STARTED, // initialized, not yet run
    TS_RUNNING, // able to run
    TS_BLOCKED, // generically unable to progress, probably a mutex
    TS_WAIT, // waiting for children to die
    TS_IOWAIT, // waiting for IO (network)
    TS_TRWAIT, // waiting for trace(2) parent.
    TS_SLEEP, // sleeping
    TS_DEAD,
};

enum thread_flags {
    TF_SYSCALL_TRACE = (1 << 0), // sys_strace
    TF_IN_SIGNAL = (1 << 1), // do_signal_call / sys_sigreturn
    TF_IS_KTHREAD = (1 << 2), // informational
    TF_USER_CTX_VALID = (1 << 3), // c_interrupt_shim
    TF_QUEUED = (1 << 4), // thread_enqueue / next_runnable_thread
    TF_ON_CPU = (1 << 5), // thread_switch
    TF_STOPPED = (1 << 6), // SIGSTOP / SIGCONT
    TF_SYSCALL_TRACE_CHILDREN = (1 << 7),
};

// These are exposed as comma-expresstions to prevent them from being
// used as lvalues - running_thread lives relative to the GS segment
// base, you can't take its address and shouldn't ever assign to it.
//
// If you need the address of the running thread's `struct thread`, use
// running_addr().
#define running_thread ((void)0, this_cpu->running)
#define running_process ((void)0, running_thread->proc)
inline struct thread *running_addr(void) { return this_cpu->running; }

// C compatibles
pid_t get_running_pid();
pid_t get_running_tid();
phys_addr_t get_running_pt_root();
void set_running_pt_root(phys_addr_t new_root);
struct dentry *get_running_cwd();
void set_running_cwd(struct dentry *new_cwd);
struct dentry *get_running_root();
elf_md *get_running_elf_metadata();
uint64_t get_running_report_events();
virt_addr_t allocate_mmap_space(size_t size);
void copy_running_mem_regions_to(struct process *to);
void for_each_thread(void (*)(struct thread *thread, void *ctx), void *ctx);
void for_each_process(void (*)(struct process *process, void *ctx), void *ctx);
// end C compatibles

void return_from_interrupt(void);
void set_kernel_stack(void *);
void threads_init(void);
struct process *process_by_id(pid_t pid);
struct thread *thread_by_id(pid_t tid);
void bootstrap_usermode(const char *init_filename);
// struct process *new_user_process(void);

struct thread *kthread_create(void (*)(void *), void *);
struct thread *thread_sched(void);
void thread_block(void);
void thread_block_irqs_disabled(void);
void thread_yield(void);
// _Noreturn void thread_done(void);

void thread_switch(
    struct thread *restrict new_thread, struct thread *restrict old_thread);
_Noreturn void thread_switch_no_save(struct thread *new_thread);
_Noreturn void kthread_exit(void);
// _Noreturn void do_thread_exit(int exit_status);
// _Noreturn void do_process_exit(int exit_status);

void block_thread(struct list *threads);

// void wake_blocked_thread(struct thread *th);
// void wake_blocked_threads(struct list *threads);
void wake_waitq_one(list *waitq);
void wake_waitq_all(list *waitq);

void kill_process_group(pid_t pgid);
void kill_process(struct process *p, int reason);

void kill_pid(pid_t pid);
void thread_enqueue(struct thread *);
void thread_enqueue_at_front(struct thread *);
void drop_thread(struct thread *);
struct thread *process_thread(struct process *);
void sleep_thread(int ms);
bool user_map(virt_addr_t base, virt_addr_t top);

void print_cpu_info(void);

END_DECLS

