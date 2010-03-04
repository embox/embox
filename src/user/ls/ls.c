/**
 * @file
 * @brief List directory contents.
 *
 * @date 02.07.2009
 * @author Anton Bondarev
 */
#include <shell_command.h>
#include <string.h>
#include <fs/rootfs.h>

#define COMMAND_NAME     "ls"
#define COMMAND_DESC_MSG "list directory contents"
#define HELP_MSG         "Usage: ls [-p path] [-h]"
static const char *man_page =
	#include "ls_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	char *path = "/";
	FSOP_DESCRIPTION *fsop;
	FS_FILE_ITERATOR iter_func;
	FILE_INFO file_info;

	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "p:h");
		switch(nextOption) {
		case 'h':
				show_help();
			return 0;
		case 'p':
			path = optarg;
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	if (NULL == (fsop = rootfs_get_fsopdesc(path))){
		LOG_ERROR("can't find fs %s\n", path);
		return 0;
	}
	if (NULL == fsop->get_file_list_iterator){
		LOG_ERROR("wrong fs desc %s\n", path);
	}
	if (NULL == (iter_func = fsop->get_file_list_iterator())){
		LOG_ERROR("can't find iterator func for fs %s\n", path);
		return 0;
	}
	printf("%16s | %8s | %10s | %10s\n", "name", "mode", "size", "size on disk");
	while (NULL != iter_func(&file_info)){
		printf("%16s | %8X | %10d | %10d\n", file_info.file_name, file_info.mode,
				file_info.size_in_bytes, file_info.size_on_disk);
	}

	return 0;
}
