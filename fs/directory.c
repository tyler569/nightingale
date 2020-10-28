#include <basic.h>
#include <ng/fs.h>
// #include <ng/fs/directory.h>
#include <ng/thread.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <list.h>

struct file_ops directory_ops = {0};

struct file *__make_directory(struct directory_file *parent, struct directory_file *new, const char *name) {
        assert(new->file.filetype == FT_DIRECTORY);
        assert(parent->file.filetype == FT_DIRECTORY);

        struct directory_node *new_node = zmalloc(sizeof(struct directory_node));
        struct directory_node *new_dot = zmalloc(sizeof(struct directory_node));
        struct directory_node *new_dotdot = zmalloc(sizeof(struct directory_node));

        list_init(&new->directory_entries);

        // TODO: INCREF

        if (!(new == fs_root_node)) {
                new_node->name = name;
                new_node->file = &new->file;
                list_append(&parent->directory_entries, &new_node->directory_siblings);
        }

        new_dot->name = ".";
        new_dot->file = &new->file;
        list_append(&new->directory_entries, &new_dot->directory_siblings);

        new_dotdot->name = "..";
        new_dotdot->file = &parent->file;
        list_append(&new->directory_entries, &new_dotdot->directory_siblings);

        return &new->file;
}

struct file *make_directory(struct file *directory, const char *name) {
        assert(directory->filetype == FT_DIRECTORY);

        struct directory_file *dir = (struct directory_file *)directory;

        struct directory_file *new = zmalloc(sizeof(struct directory_file));
        new->file.filetype = FT_DIRECTORY;
        new->file.permissions = USR_READ | USR_EXEC;
        new->file.refcnt = 1;
        new->file.ops = &directory_ops;
        list_init(&new->file.blocked_threads);

        return __make_directory(dir, new, name);
} 

struct file *fs_root_init(void) {
        __make_directory(fs_root_node, fs_root_node, "");
        return &fs_root_node->file;
}

void add_dir_file(struct file *directory, struct file *file, const char *name) {
        assert(directory->filetype == FT_DIRECTORY);

        struct directory_file *dir = (struct directory_file *)directory;
        struct directory_node *new_node = zmalloc(sizeof(struct directory_node));

        new_node->name = name;
        new_node->file = file;
        list_append(&dir->directory_entries, &new_node->directory_siblings);
}

struct file *dir_child(struct file *directory, const char *name) {
        assert(directory->filetype == FT_DIRECTORY);

        struct directory_file *dir = (struct directory_file *)directory;

        if (list_empty(&dir->directory_entries)) {
                return NULL;
        }

        list_for_each(struct directory_node, node, &dir->directory_entries, directory_siblings) {
                if (strcmp(name, node->name) == 0) {
                        return node->file;
                }
        }

        return NULL;
}

sysret sys_getdirents(int fd, struct ng_dirent *buf, ssize_t count) {
        struct open_file *ofd = dmgr_get(&running_process->fds, fd);
        if (!ofd) {
                return -EBADF;
        }
        struct file *file = ofd->node;
        if (file->filetype != FT_DIRECTORY) {
                return -ENOTDIR;
        }

        // TODO: finish
        return 0;
}
