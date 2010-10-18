/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <lib/list.h>
#include <lib/cpio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_NAME     "mount"
#define COMMAND_DESC_MSG "mount a filesystem"
#define HELP_MSG         "Usage: mount [-h] src dir"

static const char *man_page =
#include "mount_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static int exec(int argsc, char **argsv) {
	int nextOption;
	char *src, *dir;
	char fs_type[0x20];
	node_t *node;
	file_system_driver_t * drv;

	fs_type[0] = '\0';
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "ht:");
		switch (nextOption) {
		case 'h':
			show_help();
			return 0;
		case 't':
			if (0 == sscanf(optarg, "%s", fs_type)) {
				LOG_ERROR("wrong -i argument %s\n", optarg);
				return -1;
			}
			TRACE("type is %s\n", fs_type);
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (argsc > 2) {
		src = argsv[argsc - 2];
		dir = argsv[argsc - 1];
	}

	vfs_add_path(argsv[argsc - 1], NULL);
	node = vfs_find_node(argsv[argsc - 1], NULL);
	drv = find_filesystem(fs_type);
	drv->fsop->init(node);

	return 0;
}
