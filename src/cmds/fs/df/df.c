/**
 * @file
 * @brief
 *
 * @date 06.11.15
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/statvfs.h>

static void print_usage(void) {
	printf("Usage: df [OPTION]... [FILE]...\n");
}

int main(int argc, char **argv) {
	int opt;
	struct statvfs st_buf;

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			return -EINVAL;
		}
	}

	if (-1 == statvfs(argv[argc-1], &st_buf)) {
		return -errno;
	}
	printf("block dev  | file system |   size     |  mount point\n");
	printf("           |");
	printf(" %11s |", fsid2fsname(st_buf.f_fsid));
	printf(" %8lu |", st_buf.f_bsize);
	printf("           \n");

	return 0;
}
