#include <elf.h>
#include <ng/arch.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/fs/proc.h>
#include <ng/memmap.h>
#include <ng/panic.h>
#include <ng/signal.h>
#include <ng/string.h>
#include <ng/sync.h>
#include <ng/syscalls.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <ng/x86/interrupt.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

enum in_out { SCH_IN, SCH_OUT };

#define NCPUS 32
#define THREAD_STACK_SIZE 0x2000
#define THREAD_TIME milliseconds(5)
#define ZOMBIE (void *)2
#define thread_idle (this_cpu->idle_thread)

// thread_utils.c

extern struct list_head all_threads;
extern struct dmgr threads;

extern struct process proc_zero;
extern struct thread thread_zero;
extern struct cpu cpu_zero;
extern struct cpu *thread_cpus[NCPUS];
extern const char *thread_states[];

void new_cpu(int n);
void threads_init();
struct process *new_process_slot();
void free_process_slot(struct process *defunct);
struct thread *new_thread_slot();
void free_thread_slot(struct thread *defunct);
void *new_kernel_stack();
void free_kernel_stack(struct thread *th);

struct thread *thread_by_id(pid_t tid);
struct process *process_by_id(pid_t pid);
struct thread *process_thread(struct process *p);
int process_matches(pid_t wait_arg, struct process *proc);
bool user_map(virt_addr_t base, virt_addr_t top);

sysret sys_getpid();
sysret sys_gettid();
sysret sys_syscall_trace(pid_t tid, int state);
sysret sys_setpgid(int pid, int pgid);

void print_thread(struct thread *th);
void print_process(void *p);
void print_cpu_info();

sysret sys_top(int show_threads);
sysret sys_settls(void *tlsbase);
sysret sys_report_events(long event_mask);

// thread_init.c

void bootstrap_usermode(const char *init_filename);

[[noreturn]] void thread_entrypoint();
void new_userspace_entry(void *filename);
struct thread *new_thread();
struct process *new_process(struct thread *th);
struct thread *kthread_create(void (*entry)(void *), void *arg);

struct thread *new_thread_2(struct process *proc);
struct thread *new_kernel_thread_2(void (*entry)(void *), void *arg);
struct thread *new_user_thread_2(
	struct process *proc, uintptr_t entry_ip, uintptr_t stack, uintptr_t arg);
struct process *new_process_2(struct process *parent, bool fork);

sysret sys_create(const char *executable);
sysret sys_procstate(pid_t destination, enum procstate flags);

// thread_fini.c

extern struct thread *finalizer;
void make_freeable(struct thread *defunct);

[[noreturn]] void finalizer_kthread(void *);

void do_process_exit(int exit_status);
[[noreturn]] void do_thread_exit(int exit_status);

[[noreturn]] sysret sys__exit(int exit_status);
[[noreturn]] sysret sys_exit_thread(int exit_status);
[[noreturn]] sysret sys_exit_group(int exit_status);
[[noreturn]] void kthread_exit();

void kill_process(struct process *p, int reason);
void kill_pid(pid_t pid);

void destroy_child_process(struct process *proc);

// thread_fork.c

void thread_copy_flags_to_new(struct thread *new);

sysret sys_fork(struct interrupt_frame *r);
sysret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), void *new_stack,
	int flags, void *arg);

// thread_sched.c

bool enqueue_checks(struct thread *th);
void thread_enqueue(struct thread *th);
void thread_enqueue_at_front(struct thread *th);
struct thread *next_runnable_thread();
struct thread *thread_sched();

void thread_set_running(struct thread *th);
void thread_yield();
void thread_block();
void thread_block_irqs_disabled();
[[noreturn]] void thread_done();
[[noreturn]] void thread_done_irqs_disabled();

void handle_stopped_condition();
void handle_killed_condition();
bool needs_fpu(struct thread *th);
bool change_vm(struct thread *new, struct thread *old);
void account_thread(struct thread *th, enum in_out st);
void thread_switch(
	struct thread *restrict new_thread, struct thread *restrict old_thread);
[[noreturn]] void thread_switch_no_save(struct thread *new_thread);

sysret sys_yield();

// thread_wait.c

void wake_waiting_parent_thread();
struct process *find_dead_child(pid_t query);
struct thread *find_waiting_tracee(pid_t query);
void wait_for(pid_t pid);
void clear_wait();
sysret sys_waitpid(pid_t pid, int *status, enum wait_options options);

// thread_sleep.c

void unsleep_thread(struct thread *t);
void unsleep_thread_callback(void *t);
void sleep_thread(int ms);
sysret sys_sleepms(int ms);
void thread_timer(void *);
