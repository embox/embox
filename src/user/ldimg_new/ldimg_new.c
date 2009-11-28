/**
 * \file ldimg.c
 *
 * \date 03.07.2009
 * \author kse
 */
#include "shell_command.h"
#include "fs/memseg.h"
#include "fs/file_new.h"

#define COMMAND_NAME     "load_new"
#define COMMAND_DESC_MSG "load image file"
#define HELP_MSG         "Usage: load_new FILEPATH 0xADDR"
static const char *man_page =
	#include "ldimg_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int i;
	unsigned int base_addr;
	char seg_fpath[256];
	char *file_path;
	const char *file_name,*seg_name;

	if (argsc < 2) {
		show_help();
		return -1;
	}

	file_path = argsv[0];

	if (!sscanf(argsv[1], "0x%x", &base_addr))  {
		LOG_ERROR("ERROR: hex value expected.\n");
		show_help();
		return -1;
	}

	file_name = get_file_name(file_path);

	// TODO: implement strcat
	strcpy (seg_fpath,"/mm/");
	seg_name = seg_fpath+4;
	strcpy (seg_fpath+4,file_name);
	strcpy (seg_fpath+4+strlen(file_name),"_loaded");

	if (base_addr<0x40000000) {
		TRACE("segment start address must be equal or higher, than 0x40000000\n");
		return false;
	}

	if (!create_segment(seg_name, (void*)base_addr, (void*)base_addr))
		return -1;

	FDESC seg = open(seg_fpath, NULL);
	FDESC file = open(file_path, NULL);

	if ((seg==FDESC_INVALID) || (file==FDESC_INVALID))
		return -1;


	// buf optimized for whole block read (not so necessary for NOR flash)
	char buf[0x40000];
	int bytesread = 0;
	TRACE("write start\n");
	while ((bytesread = read(file,buf,0x40000)) > 0)
		if (write(seg,buf,bytesread)<=0) {
			TRACE ("WR ERR!\n");
			return -1;
		}

	TRACE("write end\n");
	return 0;
}
