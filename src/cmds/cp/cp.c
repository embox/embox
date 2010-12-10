 /**
 * @file
 * @brief Mini cp implementation for Embox.
 *
 * @date 22.08.2009
 * @author Roman Evstifeev
 */
#include <shell_command.h>
#include <fs/file.h>

#define COMMAND_NAME     "cp"
#define COMMAND_DESC_MSG "cp file"
#define HELP_MSG         "Usage: cp [-h] [source] [dest]"
static const char *man_page =
	#include "cp_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	const char *src_path,*dst_path;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (argsc < 3) {
		show_help();
		return -1;
	}

	src_path = argsv[argsc - 2];
	dst_path = argsv[argsc - 1];

	FDESC src = open(src_path, NULL);
	if (src==FDESC_INVALID)  {
		TRACE("can't open file %s\n",src_path);
		return -1;
	}

	FDESC dst = open(dst_path, O_CREAT);
	if (dst==FDESC_INVALID) {
		TRACE("can't open file %s\n",dst_path);
		return -1;
	}

	// buf optimized for whole block write
	char buf[0x40000];
	int bytesread = 0;
	while ((bytesread = read(src,buf,0x40000)) > 0)
		if (write(dst,buf,bytesread)<=0) {
			TRACE ("WR ERR!\n");
			break;
		}

	if (!fsync(dst_path))
		return -1;

	return 0;
}
