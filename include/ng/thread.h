#pragma once

#include <elf.h>
#include <list.h>
#include <ng/dmgr.h>
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

extern list all_threads;

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
	struct vnode *vnode;
};
#define NREGIONS 32

#define COMM_SIZE 32
#define PROC_MAGIC 0x434f5250 // 'PROC'
struct process {
	pid_t pid;
	pid_t pgid;
	char comm[COMM_SIZE];

	unsigned int magic; // PROC_MAGIC

	phys_addr_t vm_root;

	int uid;
	int gid;

	int exit_intention; // tells threads to exit
	int exit_status; // tells parent has exited

	struct process *parent;

	// struct dmgr fds;

	int n_files;
	struct file **files;
	struct dentry *root;

	list children;
	list threads;

	list_node siblings;

	uintptr_t mmap_base;
	struct mm_region mm_regions[NREGIONS];

	elf_md *elf_metadata;
};

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

#define THREAD_MAGIC 0x44524854 // 'THRD'

struct thread {
	pid_t tid;
	struct process *proc;

	unsigned int magic; // THREAD_MAGIC

	volatile enum thread_state state;
	enum thread_state nonsig_state; // original state before signal

    bool syscall_trace;
    bool in_signal;
    bool is_kthread;
    bool user_ctx_valid;
    bool queued;
    bool on_cpu;
    bool stopped;
    bool syscall_trace_children;

	char *kstack;

	jmp_buf kernel_ctx;
	interrupt_frame *user_ctx;

	void (*entry)(void *);
	void *entry_arg;

	struct dentry *cwd;
	struct dentry *proc_dir;

	pid_t wait_request;
	struct process *wait_result;
	struct thread *wait_trace_result;

	list tracees;
	list_node trace_node;
	struct thread *tracer;
	enum trace_state trace_state;
	int trace_report;
	int trace_signal;

	uint64_t report_events;

	list_node all_threads;
	list_node runnable;
	list_node freeable;
	list_node process_threads;
	// list_node wait_node;

	struct timer_event *wait_event;

	uintptr_t user_sp;
	jmp_buf signal_ctx;

	sighandler_t sig_handlers[32];
	sigset_t sig_pending;
	sigset_t sig_mask;

	long n_scheduled;

	// in kernel_ticks
	uint64_t time_ran;
	uint64_t last_scheduled;

	// in tsc time - divide by tsc_average_delta (TODO) -- kernel/timer
	uint64_t tsc_ran;
	uint64_t tsc_scheduled;

	int irq_disable_depth;

	int awaiting_mutex;
	int awaiting_deli_ticket;

	void *tlsbase;

	fp_ctx fpctx;
};

typedef struct thread gdb_thread_t; // for gdb type casting

// These are exposed as comma-expresstions to prevent them from being
// used as lvalues - running_thread lives relative to the GS segment
// base, you can't take its address and shouldn't ever assign to it.
//
// If you need the address of the running thread's `struct thread`, use
// running_addr().
#define running_thread ((void)0, this_cpu->running)
#define running_process ((void)0, running_thread->proc)
inline struct thread *running_addr() { return this_cpu->running; }

void return_from_interrupt();
void set_kernel_stack(void *);
void threads_init();
struct process *process_by_id(pid_t pid);
struct thread *thread_by_id(pid_t tid);
void bootstrap_usermode(const char *init_filename);
// struct process *new_user_process();

struct thread *kthread_create(void (*)(void *), void *);
struct thread *thread_sched();
void thread_block();
void thread_block_irqs_disabled();
void thread_yield();
// [[noreturn]] void thread_done();

void thread_switch(
	struct thread *restrict new_thread, struct thread *restrict old_thread);
[[noreturn]] void thread_switch_no_save(struct thread *new_thread);
[[noreturn]] void kthread_exit();
// [[noreturn]] void do_thread_exit(int exit_status);
// [[noreturn]] void do_process_exit(int exit_status);

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

void print_cpu_info();

END_DECLS
