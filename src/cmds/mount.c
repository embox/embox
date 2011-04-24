/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <lib/list.h>
#include <lib/cpio.h>
#include <stdlib.h>
#include <string.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mount [-h] src dir\n");
}

static int exec(int argc, char **argv) {
	int opt;
	char *src, *dir;
	char fs_type[0x20];
	node_t *node;
	file_system_driver_t * drv;

	fs_type[0] = '\0';
	getopt_init();
	do {
		opt = getopt(argc, argv, "ht:");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 't':
			if (0 == sscanf(optarg, "%s", fs_type)) {
				LOG_ERROR("wrong -t argument %s\n", optarg);
				return -1;
			}
			TRACE("type is %s\n", fs_type);
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	if (argc > 2) {
		src = argv[argc - 2];
		dir = argv[argc - 1];
	}

	vfs_add_path(argv[argc - 1], NULL);
	node = vfs_find_node(argv[argc - 1], NULL);
	drv = find_filesystem(fs_type);
	drv->fsop->init(node);

	return 0;
}
