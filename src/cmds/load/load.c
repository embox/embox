/**
 * @file
 * @brief Load image file into memory.
 *
 * @date 03.07.2009
 * @author Sergey Kuzmin
 */

#include <shell_command.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <stdio.h>
#include <string.h>
#include <lib/cpio.h>

#define COMMAND_NAME     "load"
#define COMMAND_DESC_MSG "load image file into memory"
#define HELP_MSG         "Usage: load [-a addr] [-f filename] [-h]"

static const char *man_page =
	#include "load_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	char *file_name = NULL;
	unsigned int load_addr;
	node_t *node;
	//FIXME: ramfs dependence
	ramfs_file_description_t *desc;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "f:a:h");
		switch (nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'f':
			file_name = optarg;
			break;
		case 'a':
			if ((optarg != NULL) && (!sscanf(optarg, "0x%x", &load_addr))
					&& (!sscanf(optarg, "%d", (int *) &load_addr))) {
				LOG_ERROR("hex value expected.\n");
				show_help();
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	node = vfs_find_node(file_name, NULL);
	desc = (ramfs_file_description_t *)node->file_info;

	TRACE("loading...addr=0x%08x, size=%d\n", desc->start_addr, desc->size);
	memcpy((void *) load_addr, desc->start_addr, desc->size);
	return 0;
}
