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

#if 0
static int copy_image(char *file_name) {
	void *romfile;
	void *ramfile;
	char romfname[0x40];
	char ramfname[0x40];
	char buff[0x1000];

	sprintf(romfname, "%s%s", "/ramfs/", file_name);
	if (NULL == (romfile = fopen(romfname, "r"))) {
		printf("Error:Can't open file %s\n", romfname);
		return -1;
	}
	sprintf(ramfname, "%s%s", romfname, "_loaded");
	if (NULL == (ramfile = fopen(ramfname, "w"))) {
		LOG_ERROR("Can't create ramfs disk");
		return -1;
	}
	while (0 < fread(buff, sizeof(buff), 1, romfile)) {
		fwrite(buff, sizeof(buff), 1, ramfile);
	}
	return 0;
}
#endif

static int exec(int argsc, char **argsv) {
#if 0
	ramfs_create_param_t param;
#endif
	fsop_desc_t *fsop;
	FILE_HANDLER *fh;
	char *file_name = NULL;
	unsigned int base_addr;
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
			if ((optarg != NULL) && (!sscanf(optarg, "0x%x", &base_addr))
					&& (!sscanf(optarg, "%d", (int *) &base_addr))) {
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

	if (NULL == (fsop = rootfs_get_fsopdesc("/ramfs"))) {
		LOG_ERROR("Can't find /ramfs disk");
		return -1;
	}
#if 0
	param.size = 0x1000000;
	param.start_addr = (unsigned int) (base_addr);
	sprintf(param.name, "%s%s", file_name, "_loaded");
	if (-1 == fsop->create_file(&param)) {
		LOG_ERROR("Can't create ramfs disk\n");
		return -1;
	}
	if (-1 == copy_image(file_name)) {
		LOG_ERROR("Can't copy image\n");
		return -1;
	}
#endif
	//TODO: workaround
	fh = fsop->open_file(file_name, "r");
	TRACE("loading...addr=0x%08x, size=%d\n", fh->fdesc->start_addr, fh->fdesc->size);
	memcpy((void *) base_addr, fh->fdesc->start_addr, fh->fdesc->size);
	return 0;
}
