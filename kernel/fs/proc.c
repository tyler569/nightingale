#include <fcntl.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/proc.h>
#include <ng/fs/vnode.h>
#include <ng/mod.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <ng/x86/vmm.h>
#include <ng/debug.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct file_system *proc_file_system;
struct file_ops proc_file_ops;
struct vnode_ops proc_vnode_ops;

static struct file_ops proc_root_dir_file_ops;
static struct vnode_ops proc_root_dir_vnode_ops;
static struct file_ops proc_thread_dir_file_ops;
static struct vnode_ops proc_thread_dir_vnode_ops;
static struct file_ops proc_fd_dir_file_ops;
static struct vnode_ops proc_fd_dir_vnode_ops;

static struct dentry *procfs_lookup_thread_dir(struct dentry *parent,
	const char *name);
static struct dentry *procfs_lookup_thread_entry(struct dentry *parent,
	const char *name);
static struct dentry *procfs_lookup_fd_entry(struct dentry *parent,
	const char *name);
static ssize_t procfs_root_getdents(struct file *file, struct dirent *buf,
	size_t len);
static ssize_t procfs_thread_getdents(struct file *file, struct dirent *buf,
	size_t len);
static ssize_t procfs_fd_getdents(struct file *file, struct dirent *buf,
	size_t len);

static struct dentry *procfs_find_child(
	struct dentry *parent, const char *name) {
	list_for_each_safe (&parent->children) {
		struct dentry *child
			= container_of(struct dentry, children_node, it);
		if (child->name && strcmp(child->name, name) == 0)
			return child;
	}
	return nullptr;
}

static struct dentry *procfs_make_child(struct dentry *parent,
	const char *name, struct vnode *vnode, bool ephemeral) {
	struct dentry *child = new_dentry();
	child->name = strdup(name);
	child->parent = parent;
	child->file_system = parent->mounted_file_system ? parent->mounted_file_system
													: parent->file_system;
	if (ephemeral)
		child->flags = DENTRY_EPHEMERAL;
	list_append(&parent->children, &child->children_node);
	attach_vnode(child, vnode);
	return child;
}

static bool procfs_parse_positive_int(const char *name, int *out) {
	int value = 0;
	if (!name || !name[0])
		return false;
	for (size_t i = 0; name[i]; i++) {
		char c = name[i];
		if (c < '0' || c > '9')
			return false;
		value = value * 10 + (c - '0');
	}
	*out = value;
	return true;
}

static struct thread *procfs_thread_by_tid(pid_t tid) {
	struct thread *th = thread_by_id(tid);
	if (!th || th == ZOMBIE)
		return nullptr;
	return th;
}

static const char *procfs_thread_state_name(enum thread_state state) {
	switch (state) {
	case TS_INVALID:
		return "invalid";
	case TS_PREINIT:
		return "preinit";
	case TS_STARTED:
		return "started";
	case TS_RUNNING:
		return "running";
	case TS_BLOCKED:
		return "blocked";
	case TS_WAIT:
		return "wait";
	case TS_IOWAIT:
		return "iowait";
	case TS_TRWAIT:
		return "tracewait";
	case TS_SLEEP:
		return "sleep";
	case TS_DEAD:
		return "dead";
	default:
		return "unknown";
	}
}

static bool procfs_check_bp(uintptr_t bp) {
	if (bp < 0x1000)
		return false;
	if (vmm_virt_to_phy(bp) == ~0u)
		return false;
	if (vmm_virt_to_phy(bp + 8) == ~0u)
		return false;
	return true;
}

static void procfs_print_frame(struct file *ofd, struct process *proc,
	uintptr_t bp, uintptr_t ip) {
	struct mod_sym sym = elf_find_symbol_by_address(ip);
	if (ip > HIGHER_HALF && sym.sym) {
		const elf_md *md = sym.mod ? sym.mod->md : &elf_ngk_md;
		const char *name = elf_symbol_name(md, sym.sym);
		ptrdiff_t offset = ip - sym.sym->st_value;
		if (sym.mod) {
			proc_sprintf(ofd, "(%#018zx) <%s:%s+%#tx> (%s @ %#018tx)\n", ip,
				sym.mod->name, name, offset, sym.mod->name, sym.mod->load_base);
		} else {
			proc_sprintf(ofd, "(%#018zx) <%s+%#tx>\n", ip, name, offset);
		}
	} else if (ip != 0) {
		const elf_md *md = proc ? proc->elf_metadata : nullptr;
		if (!md) {
			proc_sprintf(ofd, "(%#018zx) <?+?>\n", ip);
			return;
		}
		const Elf_Sym *sym = elf_symbol_by_address(md, ip);
		if (!sym) {
			proc_sprintf(ofd, "(%#018zx) <?+?>\n", ip);
			return;
		}
		const char *name = elf_symbol_name(md, sym);
		ptrdiff_t offset = ip - sym->st_value;
		proc_sprintf(ofd, "(%#018zx) <%s+%#tx>\n", ip, name, offset);
	}
}

static void procfs_backtrace(struct file *ofd, struct thread *th) {
	uintptr_t bp = th->kernel_ctx->__regs.bp;
	uintptr_t ip = th->kernel_ctx->__regs.ip;
	int frame_count = 0;

	while (bp) {
		procfs_print_frame(ofd, th->proc, bp, ip);

		ip = *(uintptr_t *)(bp + 8);
		bp = *(uintptr_t *)bp;

		if (!procfs_check_bp(bp))
			break;

		if (frame_count++ > 45) {
			proc_sprintf(ofd, "(too many frames)\n");
			break;
		}
	}
}

static void procfs_thread_stack(struct file *ofd, void *arg) {
	pid_t tid = (pid_t)(uintptr_t)arg;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th) {
		proc_sprintf(ofd, "no such thread\n");
		return;
	}
	procfs_backtrace(ofd, th);
}

static void procfs_thread_status(struct file *ofd, void *arg) {
	pid_t tid = (pid_t)(uintptr_t)arg;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th) {
		proc_sprintf(ofd, "no such thread\n");
		return;
	}
	struct process *proc = th->proc;
	proc_sprintf(ofd, "tid:\t%i\n", th->tid);
	proc_sprintf(ofd, "pid:\t%i\n", proc->pid);
	proc_sprintf(ofd, "pgid:\t%i\n", proc->pgid);
	proc_sprintf(ofd, "uid:\t%i\n", proc->uid);
	proc_sprintf(ofd, "gid:\t%i\n", proc->gid);
	proc_sprintf(ofd, "state:\t%s\n", procfs_thread_state_name(th->state));
	proc_sprintf(ofd, "comm:\t%s\n", proc->comm);
}

static void procfs_thread_comm(struct file *ofd, void *arg) {
	pid_t tid = (pid_t)(uintptr_t)arg;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th) {
		proc_sprintf(ofd, "no such thread\n");
		return;
	}
	proc_sprintf(ofd, "%s\n", th->proc->comm);
}

static const char *procfs_exe_target(struct process *proc) {
	if (proc->exe_path[0])
		return proc->exe_path;
	if (proc->comm[0])
		return proc->comm;
	return "unknown";
}

static struct dentry *procfs_make_symlink(
	struct dentry *parent, const char *name, const char *target) {
	struct vnode *vnode = new_vnode(proc_file_system, _NG_SYMLINK | 0777);
	vnode->symlink_destination = strdup(target);
	return procfs_make_child(parent, name, vnode, true);
}

static ssize_t procfs_emit_dirent(struct dirent *buf, size_t len,
	size_t *offset, const char *name, unsigned short mode, unsigned char type,
	ino_t ino) {
	size_t max_copy = MIN(256ul, len - sizeof(struct dirent) - *offset);
	size_t str_len = strlen(name);
	size_t will_copy = MIN(str_len, max_copy);
	if (will_copy < str_len)
		return -1;
	struct dirent *dent = PTR_ADD(buf, *offset);
	strncpy(dent->d_name, name, max_copy);
	dent->d_type = type;
	dent->d_mode = mode;
	dent->d_ino = ino;
	dent->d_off = 0;
	size_t reclen = sizeof(struct dirent) - 256 + ROUND_UP(will_copy + 1, 8);
	dent->d_reclen = reclen;
	*offset += reclen;
	return reclen;
}

static ssize_t procfs_root_getdents(
	struct file *file, struct dirent *buf, size_t len) {
	size_t offset = 0;
	size_t index = 0;
	size_t start = file->offset;

	list_for_each_safe (&file->dentry->children) {
		struct dentry *d = container_of(struct dentry, children_node, it);
		if (!d->vnode || (d->flags & DENTRY_EPHEMERAL))
			continue;
		if (index++ < start)
			continue;
		if (procfs_emit_dirent(buf, len, &offset, d->name,
				(unsigned short)d->vnode->mode, d->vnode->type,
				d->vnode->vnode_number)
			< 0)
			break;
	}

	list_for_each_safe (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		char name[16];
		snprintf(name, sizeof(name), "%i", th->tid);
		if (index++ < start)
			continue;
		if (procfs_emit_dirent(buf, len, &offset, name, 0555, FT_DIRECTORY,
				(ino_t)th->tid)
			< 0)
			break;
	}

	file->offset = index;
	return offset;
}

static ssize_t procfs_thread_getdents(
	struct file *file, struct dirent *buf, size_t len) {
	size_t offset = 0;
	size_t index = 0;
	size_t start = file->offset;
	static const struct {
		const char *name;
		unsigned short mode;
		unsigned char type;
	} entries[] = {
		{ "stack", 0444, FT_PROC },
		{ "status", 0444, FT_PROC },
		{ "comm", 0444, FT_PROC },
		{ "exe", 0777, FT_SYMLINK },
		{ "fd", 0555, FT_DIRECTORY },
	};

	for (size_t i = 0; i < sizeof(entries) / sizeof(entries[0]); i++) {
		if (index++ < start)
			continue;
		if (procfs_emit_dirent(buf, len, &offset, entries[i].name,
				entries[i].mode, entries[i].type, (ino_t)i + 1)
			< 0)
			break;
	}

	file->offset = index;
	return offset;
}

static ssize_t procfs_fd_getdents(
	struct file *file, struct dirent *buf, size_t len) {
	size_t offset = 0;
	size_t index = 0;
	size_t start = file->offset;
	pid_t tid = (pid_t)(uintptr_t)file->vnode->data;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th)
		return 0;

	spin_lock(&th->proc->fds.lock);
	for (int fd = 0; fd < th->proc->fds.cap; fd++) {
		if (!th->proc->fds.data[fd])
			continue;
		char name[16];
		snprintf(name, sizeof(name), "%i", fd);
		if (index++ < start)
			continue;
		if (procfs_emit_dirent(buf, len, &offset, name, 0777, FT_SYMLINK,
				(ino_t)fd + 1)
			< 0)
			break;
	}
	spin_unlock(&th->proc->fds.lock);

	file->offset = index;
	return offset;
}

static struct dentry *procfs_lookup_thread_dir(
	struct dentry *parent, const char *name) {
	struct dentry *existing = procfs_find_child(parent, name);
	if (existing)
		return existing;
	int tid = 0;
	if (!procfs_parse_positive_int(name, &tid))
		return add_child(parent, name, nullptr);
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th)
		return TO_ERROR(-ENOENT);

	struct vnode *vnode = new_vnode(proc_file_system, _NG_DIR | 0555);
	vnode->ops = &proc_thread_dir_vnode_ops;
	vnode->file_ops = &proc_thread_dir_file_ops;
	vnode->data = (void *)(uintptr_t)tid;
	return procfs_make_child(parent, name, vnode, true);
}

static struct dentry *procfs_lookup_thread_entry(
	struct dentry *parent, const char *name) {
	pid_t tid = (pid_t)(uintptr_t)parent->vnode->data;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th)
		return TO_ERROR(-ENOENT);

	if (strcmp(name, "stack") == 0) {
		struct vnode *vnode
			= new_proc_vnode(0444, procfs_thread_stack, (void *)(uintptr_t)tid);
		return procfs_make_child(parent, name, vnode, true);
	}
	if (strcmp(name, "status") == 0) {
		struct vnode *vnode
			= new_proc_vnode(0444, procfs_thread_status, (void *)(uintptr_t)tid);
		return procfs_make_child(parent, name, vnode, true);
	}
	if (strcmp(name, "comm") == 0) {
		struct vnode *vnode
			= new_proc_vnode(0444, procfs_thread_comm, (void *)(uintptr_t)tid);
		return procfs_make_child(parent, name, vnode, true);
	}
	if (strcmp(name, "exe") == 0)
		return procfs_make_symlink(parent, name, procfs_exe_target(th->proc));
	if (strcmp(name, "fd") == 0) {
		struct vnode *vnode = new_vnode(proc_file_system, _NG_DIR | 0555);
		vnode->ops = &proc_fd_dir_vnode_ops;
		vnode->file_ops = &proc_fd_dir_file_ops;
		vnode->data = (void *)(uintptr_t)tid;
		return procfs_make_child(parent, name, vnode, true);
	}

	return TO_ERROR(-ENOENT);
}

static struct dentry *procfs_lookup_fd_entry(
	struct dentry *parent, const char *name) {
	int fd = 0;
	if (!procfs_parse_positive_int(name, &fd))
		return TO_ERROR(-ENOENT);

	pid_t tid = (pid_t)(uintptr_t)parent->vnode->data;
	struct thread *th = procfs_thread_by_tid(tid);
	if (!th)
		return TO_ERROR(-ENOENT);

	spin_lock(&th->proc->fds.lock);
	struct file *file = (fd >= 0 && fd < th->proc->fds.cap)
		? th->proc->fds.data[fd]
		: nullptr;
	if (!file) {
		spin_unlock(&th->proc->fds.lock);
		return TO_ERROR(-ENOENT);
	}

	char target[256] = { 0 };
	if (file->dentry) {
		pathname(file->dentry, target, sizeof(target));
	} else if (file->vnode && (file->vnode->is_anon_pipe
								  || file->vnode->type == FT_PIPE)) {
		strncpy(target, "pipe", sizeof(target));
	} else {
		strncpy(target, "anon", sizeof(target));
	}
	spin_unlock(&th->proc->fds.lock);

	return procfs_make_symlink(parent, name, target);
}

struct vnode *new_proc_vnode(
	int mode, void (*generate)(struct file *, void *arg), void *arg) {
	struct vnode *vnode = new_vnode(proc_file_system, _NG_PROC | mode);
	vnode->ops = &proc_vnode_ops;
	vnode->file_ops = &proc_file_ops;
	vnode->extra = generate;
	vnode->data = arg;
	return vnode;
}

void make_proc_file(
	const char *name, void (*generate)(struct file *, void *arg), void *arg) {
	struct dentry *root = proc_file_system->root;
	struct vnode *vnode = new_proc_vnode(0444, generate, arg);
	struct dentry *dentry = resolve_path_from(root, name, true);
	if (dentry->vnode) {
		printf("proc file '%s' already exists\n", name);
		maybe_delete_vnode(vnode);
		return;
	}
	attach_vnode(dentry, vnode);
}

ssize_t proc_file_read(struct file *file, char *buffer, size_t len) {
	size_t to_read = MIN(file->len - file->offset, len);
	memcpy(buffer, PTR_ADD(file->extra, file->offset), to_read);
	file->offset += to_read;
	return to_read;
}

ssize_t proc_file_write(struct file *file, const char *buffer, size_t len) {
	return -ETODO;
}

off_t proc_file_seek(struct file *file, off_t offset, int whence) {
	off_t new_offset = file->offset;

	switch (whence) {
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset += offset;
		break;
	case SEEK_END:
		// The only difference between this and default_seek is that
		// this uses file->len for SEEK_END because the vnode has no
		// len of its own.
		new_offset = file->len + offset;
		break;
	default:
		return -EINVAL;
	}

	if (new_offset < 0)
		return -EINVAL;

	file->offset = new_offset;
	return new_offset;
}

int proc_file_open(struct vnode *vnode, struct file *file) {
	void (*generate)(struct file *, void *arg);
	file->extra = malloc(4096 * 4);
	file->size = 4096 * 4;
	file->len = 0;
	generate = vnode->extra;
	generate(file, vnode->data);
	return 0;
}

int proc_file_close(struct vnode *vnode, struct file *file) {
	if (file->extra)
		free(file->extra);
	return 0;
}

struct file_ops proc_file_ops = {
	.read = proc_file_read,
	.write = proc_file_write,
	.seek = proc_file_seek,
};

struct vnode_ops proc_vnode_ops = {
	.open = proc_file_open,
	.close = proc_file_close,
};

static struct vnode_ops proc_root_dir_vnode_ops = {
	.lookup = procfs_lookup_thread_dir,
};

static struct file_ops proc_root_dir_file_ops = {
	.getdents = procfs_root_getdents,
};

static struct vnode_ops proc_thread_dir_vnode_ops = {
	.lookup = procfs_lookup_thread_entry,
};

static struct file_ops proc_thread_dir_file_ops = {
	.getdents = procfs_thread_getdents,
};

static struct vnode_ops proc_fd_dir_vnode_ops = {
	.lookup = procfs_lookup_fd_entry,
};

static struct file_ops proc_fd_dir_file_ops = {
	.getdents = procfs_fd_getdents,
};

void proc_sprintf(struct file *file, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	file->len += vsnprintf(
		file->extra + file->len, file->size - file->len, fmt, args);
	// va_end is called in vsnprintf
}

void procfs_setup_root() {
	struct vnode *root_vnode = proc_file_system->root->vnode;
	root_vnode->ops = &proc_root_dir_vnode_ops;
	root_vnode->file_ops = &proc_root_dir_file_ops;
}
