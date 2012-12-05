/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include <embox/cmd.h>
#include <fs/vfs.h>
#include <fs/fs_drv.h>
#include <fs/sys/fsop.h>/* now mount declaration in this header */

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mount [-h] [-t fstype] dev dir\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int opt_cnt;
	char *dev, *dir;
	char *fs_type;

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
		if(argc > 3) {
			if(opt_cnt) {
				fs_type = argv[argc - 3];
			}
			else {
				print_usage();
				return 0;
			}
		}

		if(0 == fs_type) {
			print_usage();
			printf("try to set \"-t [fstype]\" option\n");
			return 0;
		}

		return mount(dev, dir, fs_type);
	}
	return 0;
}
