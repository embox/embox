/**
 * \file ls_new.c
 */
#include "shell_command.h"
#include "string.h"
#include "fs/file_new.h"
#include "fs/memseg.h"

#define COMMAND_NAME     "ls_new"
#define COMMAND_DESC_MSG "lists files or segments"
#define HELP_MSG         "Usage: ls_new [-s] [path] [-h]"
static const char *man_page =
	#include "ls_help.inc"
	;

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

#define MAX_NITEMS 100

static int exec(int argsc, char **argsv) {
	if (argsc < 1) {
		show_help();
		return -1;
	}

	if (!strcmp (argsv[0],"-s")) {
		SEGMENT seglist[MAX_NITEMS];
		int slsz = list_segments(seglist, MAX_NITEMS);
		if (slsz<0) {
			printf("list segments failed\n");
			return -1;
		}
		printf("%16s | %10s | %10s | %10s\n", "name", "start", "end", "size");
		int i;
		for (i=0;i<MAX_NITEMS;i++) {
			int segsize = seglist[i].end - seglist[i].start;
			printf("%16s | 0x%08x | 0x%08x | %10d\n",
			   seglist[i].name, seglist[i].start, seglist[i].end, segsize);
		}

		if (MAX_NITEMS < slsz)
			printf("and there is more...\n");

		return 0;
	}

	char *path = argsv[0];

	FLIST_ITEM flist[MAX_NITEMS];

   size_t lsize = list_dir(path, flist, MAX_NITEMS);
	if (lsize<0) {
		printf("invalid path\n");
		return -1;
	}

	printf("%16s | %10s\n", "name", "size");
	int i;
	for (i=0;i<MAX_NITEMS;i++)
		printf("%16s | %10d\n", flist[i].name, flist[i].size);

	if (MAX_NITEMS < lsize)
		printf("and there is more...\n");

	return 0;
}
