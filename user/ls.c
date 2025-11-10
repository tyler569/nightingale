#include <dirent.h>
#include <fcntl.h>
#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void check_err(int code, const char *message) {
	if (code < 0) {
		perror(message);
		exit(EXIT_FAILURE);
	}
}

char ft_sigil(struct dirent *dirent) {
	int type = dirent->d_type;
	int perm = dirent->d_mode;
	if (type == FT_NORMAL && (perm & USR_EXEC)) {
		return '*';
	} else {
		return __filetype_sigils[type];
	}
}

void print_permission_block(struct dirent *dirent) {
	switch (dirent->d_type) {
	case FT_SYMLINK:
		printf("l");
		break;
	case FT_DIRECTORY:
		printf("d");
		break;
	default:
		printf("-");
		break;
	}

	static const char *bits = "rwxrwxrwx";

	for (int i = 0; i < 9; i++) {
		int bit = 8 - i;
		bool p = (dirent->d_mode >> bit) & 1;
		if (p)
			printf("%c", bits[i]);
		else
			printf("-");
	}
}

void help(const char *progname) {
	fprintf(stderr,
		"%s: usage ls [-alF]\n"
		"  -a     Show all files\n"
		"  -l     List files long form\n"
		"  -F     Show filetype sigils\n",
		progname);
}

int compare_dirent_ptrs(const void *a, const void *b) {
	return strcmp(
		(*(struct dirent **)a)->d_name, (*(struct dirent **)b)->d_name);
}

struct dirent *dirent_ptrs[128];
char *dirent_buf[8192];
struct stat statbuf;

int main(int argc, char **argv) {
	bool all = false;
	// Intentionally checking this before potentially redirecting output
	bool classify = isatty(STDOUT_FILENO);
	bool long_ = !classify;
	int fd, opt;

	while ((opt = getopt(argc, argv, "alF")) != -1) {
		switch (opt) {
		case 'a':
			all = true;
			break;
		case 'l':
			long_ = true;
			break;
		case 'F':
			classify = true;
			break;
		default:
			help(argv[0]);
			return 0;
		}
	}

	if (!argv[optind]) {
		fd = open(".", O_RDONLY);
	} else {
		// TODO: loop over remaining arguments and list them all
		fd = open(argv[optind], O_RDONLY);
	}
	check_err(fd, "open");

	int size = getdents(fd, (void *)dirent_buf, 8192);
	check_err(size, "readdir");

	size_t n_dirents = 0;
	size_t size_val = size;
	for (size_t i = 0; i < size_val;) {
		struct dirent *d = PTR_ADD(dirent_buf, i);
		dirent_ptrs[n_dirents++] = d;
		i += d->d_reclen;
	}

	qsort(dirent_ptrs, n_dirents, sizeof(struct dirent *), compare_dirent_ptrs);

	if (!long_) {
		redirect_output_to((char *[]) { "/bin/column", nullptr });
	}

	char buffer[256] = { 0 };

	for (size_t i = 0; i < n_dirents; i++) {
		struct dirent *entry = dirent_ptrs[i];
		if (entry->d_name[0] == '.') {
			if (!all)
				continue;
		}

		if (long_) {
			statat(fd, entry->d_name, &statbuf);
			print_permission_block(entry);
			printf(" %1i", statbuf.st_nlink);
			printf(" %1i", statbuf.st_uid);
			printf(" %1i", statbuf.st_gid);
			printf(" %8li", statbuf.st_size);
			struct tm tm;
			gmtime_r(&statbuf.st_mtime, &tm);
			strftime(buffer, 256, "%b %e %H:%M", &tm);
			printf(" %s", buffer);
			printf(" %s", entry->d_name);
			if (entry->d_type == FT_SYMLINK) {
				int err = readlinkat(fd, entry->d_name, buffer, 256);
				if (err < 0) {
					perror("readlinkat");
					printf("\n");
					continue;
				}
				printf(" -> \x1b[31m%s\x1b[m", buffer);
			}
			printf("\n");
		} else {
			if (classify) {
				printf("%s%c\n", entry->d_name, ft_sigil(entry));
			} else {
				printf("%s\n", entry->d_name);
			}
		}
	}

	return EXIT_SUCCESS;
}
