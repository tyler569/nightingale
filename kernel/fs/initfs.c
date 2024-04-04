#include <ctype.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/init.h>
#include <ng/fs/inode.h>
#include <stdio.h>
#include <tar.h>

static uint64_t tar_convert_number(char *num);

void make_tar_file(
	struct dentry *dentry, int mode, size_t len, time_t mtime, void *content) {
	struct inode *inode = new_inode_notime(dentry->file_system, mode);
	inode->data = content;
	inode->len = len;
	inode->capacity = len;
	inode->atime = inode->mtime = inode->ctime = mtime;
	attach_inode(dentry, inode);
}

void load_initfs(void *initfs) {
	struct tar_header *tar = initfs;

	printf("tar: %p\n", tar);

	while (tar->filename[0]) {
		size_t len = tar_convert_number(tar->size);
		int mode = (int)tar_convert_number(tar->mode);
		time_t mtime = (time_t)tar_convert_number(tar->mtime);
		mode &= 07555; // no writing to tarfs files;

		void *content = ((char *)tar) + 512;
		const char *filename = tar->filename;

		struct dentry *dentry = resolve_path(filename);
		if (IS_ERROR(dentry) && tar->typeflag != XATTR) {
			printf("cannot resolve '%s' while populating initfs\n", filename);
		}

		if (tar->typeflag == REGTYPE || tar->typeflag == AREGTYPE) {
			make_tar_file(dentry, mode, len, mtime, content);
		} else if (tar->typeflag == DIRTYPE) {
			make_tar_file(dentry, _NG_DIR | mode | 0200, 0, mtime, NULL);
		} else if (tar->typeflag == XATTR) {
			// ignore POSIX extended attributes
		} else {
			printf("warning: tar file of unknown type '%c'\n", tar->typeflag);
		}

		tar = PTR_ADD(tar, ROUND_UP(len + 512, 512));
	}
}

static uint64_t tar_convert_number(char *num) {
	uint64_t value = 0;

	for (size_t place = 0; isdigit(num[place]); place += 1) {
		uint64_t part = num[place] - '0';
		value <<= 3;
		value += part;
	}

	return value;
}
