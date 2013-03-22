/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <embox/cmd.h>
#include <fs/sys/fsop.h>
#include <fs/file_system.h>
#include <mem/phymem.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mount [-h] [-t fstype] dev dir\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int opt_cnt;
	char *dev, *dir;
	char *fs_type;
	char *buff;

	opt_cnt = 0;
	fs_type = 0;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
		case 't':
			opt_cnt++;
			break;
		case '?':
			break;
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (argc > 2) {
		dev = argv[argc - 2];
		dir = argv[argc - 1];
		if (argc > 3) {
			if (opt_cnt) {
				fs_type = argv[argc - 3];
			} else {
				print_usage();
				return 0;
			}
		}

		if (0 == fs_type) {
			print_usage();
			printf("try to set \"-t [fstype]\" option\n");
			return 0;
		}

		return mount(dev, dir, fs_type);
	}

	buff = page_alloc(__phymem_allocator, 1);
	filesystem_get_mount_list(buff);
	printf("%s", buff);
	page_free(__phymem_allocator, buff, 1);

	return 0;
}
