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
#define HELP_MSG         "Usage: ls [-hl] path"

static const char *man_page =
	#include "ls_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void print_list(FS_FILE_ITERATOR iter_func) {
	FILE_INFO file_info;
	while (NULL != iter_func(&file_info)){
		printf("%s\n", file_info.file_name);
	}
}

static void print_long_list(FS_FILE_ITERATOR iter_func) {
	FILE_INFO file_info;
	char mode[] = "---";
	printf("%16s | %4s | %10s | %10s\n", "name", "mode", "size", "size on disk");
	while (NULL != iter_func(&file_info)){
		if(file_info.mode & FILE_MODE_RO) {
			mode[0] = 'r';
		}
		if(file_info.mode & FILE_MODE_WO) {
			mode[1] = 'w';
		}
		if(file_info.mode & FILE_MODE_XO) {
			mode[2] = 'x';
		}
		printf("%16s |  %3s | %10d | %10d\n", file_info.file_name, mode,
				file_info.size_in_bytes, file_info.size_on_disk);
	}
}

static int exec(int argsc, char **argsv) {
	char *path = "/";
	int long_list = 0;
	fsop_desc_t *fsop;
	FS_FILE_ITERATOR iter_func;

	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "lh");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'l':
			long_list = 1;
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	if(argsc > 1) {
		path = argsv[argsc - 1];
	}

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

	if(long_list) {
		print_long_list(iter_func);
	} else {
		print_list(iter_func);
	}

	return 0;
}
