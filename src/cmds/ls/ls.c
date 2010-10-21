/**
 * @file
 * @brief List directory contents.
 *
 * @date 02.07.2009
 * @author Anton Bondarev
 */
#include <shell_command.h>
#include <string.h>
#include <time.h>
#include <fs/file.h>
#include <fs/vfs.h>
#include <fs/ramfs.h>

#define COMMAND_NAME     "ls"
#define COMMAND_DESC_MSG "list directory contents"
#define HELP_MSG         "Usage: ls [-hl] path"

static const char *man_page =
	#include "ls_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void print_long_list(char *path, node_t *nod, int recursive) {
	struct list_head *p;
	node_t *item;
	//TODO: workaround.
	ramfs_file_description_t *desc;
	char time_buff[17];
	printf("%s\t%s\t%s\t\t\t%s\n", "mode", "size", "mtime", "name");

	list_for_each(p, &(nod->leaves)) {
		item = (node_t*)list_entry(p, node_t, neighbors);
		desc = (ramfs_file_description_t *)item->file_info;
		ctime(&desc->mtime, time_buff);
		printf("%d\t%d\t%s\t%s\n",
			desc->mode,
			desc->size,
			time_buff,
			(char *)item->name);
	}
}

static void print_folder(char *path, node_t *nod, int recursive) {
	struct list_head *p;
	list_for_each(p, &(nod->leaves)) {
		if (recursive) {
			if(0 == strcmp(path, "/")) {
				printf("%s\n",  (char *)((node_t*)list_entry(p, node_t, neighbors))->name);
			} else {
				printf("%s/%s\n", path, (char *)((node_t*)list_entry(p, node_t, neighbors))->name);
				strcat(path, (char *)((node_t*)list_entry(p, node_t, neighbors))->name);
				print_folder(path, (node_t*)list_entry(p, node_t, neighbors), recursive);
			}
		} else {
			printf("%s\n", (char *)((node_t*)list_entry(p, node_t, neighbors))->name);
		}
	}
}

typedef void (*print_func_t)(char *path, node_t *nod, int recursive);

static int exec(int argsc, char **argsv) {
	int long_list = 0;
	node_t *nod;
	char path[CONFIG_MAX_LENGTH_FILE_NAME];

	int recursive = 0;
	volatile print_func_t print_func = print_folder;

	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "Rlh");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'l':
			long_list = 1;
			print_func = print_long_list;
			break;
		case 'R':
			recursive = 1;
			print_func = print_folder;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	if(argsc > 1) {
		sprintf(path, "%s", argsv[argsc - 1]);
	}
	else {
		sprintf(path, "/");
	}

	nod = vfs_find_node(path, NULL);
	print_func(path, nod, recursive);

	return 0;
}
