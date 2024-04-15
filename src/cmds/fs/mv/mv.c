/**
 * @file
 * @brief Move or rename file.
 *
 * @date 18.12.12
 * @author Ivan Tretyakov
 */


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <lib/libds/array.h>

#define MV_RECURSIVE	(0x1 << 0)
#define MV_FORCE		(0x1 << 1)
#define DESC_NOT_EXIST  (0x1 << 2)
#define DESC_IS_DIR     (0x1 << 3)

/* Iterate through array from position 'start', 'count' times
 * and set 'value' to current value on each step */

static void print_usage(void) {
	printf("Usage: mv [-rfh] path\n"
			"\t-f - do not prompt before overwrite\n"
			"\t-h - print this help\n");
}

int main(int argc, char **argv) {
	int opt;
	int flags;
	int opt_cnt;
	struct stat sb;

	flags = 0;
	opt_cnt = 0;

	while (-1 != (opt = getopt(argc, argv, "fh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return ENOERR;
		case 'f':
			flags |= MV_FORCE;
			break;
		default:
			printf("mv: invalid option -- '%c'\n", optopt);
			return -EINVAL;
		}
		opt_cnt ++;
	}
	/* if desc is directory */
	if (-1 == stat(argv[argc-1], &sb)) {
		flags |= DESC_NOT_EXIST;
	} else {
		switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:
			flags |= DESC_IS_DIR;
			break;
		case S_IFREG:
			if (!(flags & MV_FORCE)) {
				printf("file '%s' already exist use -f for rewrite it\n", argv[argc-1]);
				return -EINVAL;
			} else {
				remove(argv[argc-1]);
			}
			break;
		default:
			printf("dest must be directory or regular file\n");
			return -EINVAL;
		}
	}
	/* if there are several source files dest must be a directory */
	if (((argc - opt_cnt) > 3) && !(flags & DESC_IS_DIR)) {
		if (!(flags & DESC_NOT_EXIST)) {
			printf("dest '%s' is not a directory\n", argv[argc-1]);
			return -EINVAL;
		} else {
			if(mkdir(argv[argc-1], 0777)) {
				printf("can't create directory %s\n", argv[argc-1]);
				return -EINVAL;
			}
			flags |= DESC_IS_DIR;
		}
	}
	if (flags & DESC_IS_DIR) {
		printf("now directory isn't supported");
		return -ENOSUPP;
	}

	if (rename(argv[argc-2], argv[argc-1])) {
		return -errno;
	}

	return ENOERR;
}
