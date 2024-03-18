/**
 * @file
 * @brief unmount a filesystem
 *
 * @date 21.02.13
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>

static void print_usage(void) {
	printf("Usage: umount [-h] dir\n");
}

int main(int argc, char **argv) {
	int opt;
	const char *dir;

	while (-1 != (opt = getopt(argc, argv, "h:"))) {
		switch (opt) {
		case '?':
			break;
		case 'h':
			print_usage();
			return 0;
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (argc != 2) {
		print_usage();
		return EINVAL;
	}

	dir = argv[argc - 1];

	if (-1 == umount(dir)) {
		printf("umount: %s\n", strerror(errno));
	}

	return 0;
}
