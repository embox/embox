/**
 * \file ldimg.c
 *
 * \date 03.07.2009
 * \author kse
 */
#include "asm/types.h"
#include "shell_command.h"
#include "ldimg.h"

#include "rootfs.h"
#include "ramfs.h"
#include "file.h"


static char ldimg_keys[] = {
#include "ldimg_keys.inc"
		};

static void show_help() {
	printf(
#include "ldimg_help.inc"
	);
}
#define COMMAND_NAME "load"
#define COMMAND_DESC_MSG "load image file"
static const char *help_msg =
	#include "ldimg_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);


#define ENTRY_PTR 0x40000000


int copy_image(file_name)
{

//    extern char _piggy_start, _piggy_end, _data_start;
//
//    char *src = &_piggy_start;
//    char *dst = (char*)ENTRY_PTR;
//
//    if (&_piggy_start == &_piggy_end) {
//        printf("No any images for ldimg in this build.\n");
//        return;
//    }
//
//
//    printf("\nCopy piggy image\n");
//    printf("from %08X to %08X size %d bytes\n", (unsigned)src, (unsigned)dst, (unsigned)(&_piggy_end) - (unsigned)(&_piggy_start));
//    while (src < &_piggy_end) {
//        *dst++ = *src++;
//    }
    void *romfile;
    void *ramfile;
    char romfname[0x40];
    char ramfname[0x40];
    char buff[0x1000];


    sprintf(romfname, "%s%s", "/ramfs/", file_name);
    if (NULL == (romfile = fopen(romfname, "r"))){
        printf ("Error:Can't open file %s\n", romfname);
        return -1;
    }
    sprintf(ramfname, "%s%s", romfname, "_loaded");
    if (NULL == (ramfile = fopen(ramfname,"w"))){
        printf ("Error:Can't create ramfs disk");
        return -1;
    }
    while(0 < fread(buff, sizeof(buff), 1, romfile)){
        fwrite(buff, sizeof(buff), 1, ramfile);
    }
}


//int ldimg_shell_handler(int argsc, char **argsv) {
static int exec(int argsc, char **argsv){
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;
	int i;
	//SHELL_HANDLER_DESCR *shell_handlers;
    RAMFS_CREATE_PARAM param;
	char *file_name;
	unsigned int base_addr;
	char ramfname[0x40];


	FSOP_DESCRIPTION *fsop;

	keys_amount = parse_arg("ldimg", argsc, argsv, ldimg_keys, sizeof(ldimg_keys),
			keys);

	if (keys_amount < 0) {
	        show_help();
	        return -1;
	    }
	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}
    if (get_key('f', keys, keys_amount, &key_value)) {
        file_name = key_value;
    }



    if (get_key('a', keys, keys_amount, &key_value)) {
        if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &base_addr))
                && (!sscanf(key_value, "%d", (int *) &base_addr))) {
            LOG_ERROR("ERROR: hex value expected.\n");
            show_help();
            return -1;
        }
    }

	if (NULL == (fsop = rootfs_get_fsopdesc("/ramfs/"))){
	    printf ("Error:Can't find ramfs disk");
	    return -1;
	}

    param.size = 0x1000000;
    param.start_addr = (unsigned int )(base_addr);
    sprintf(param.name, "%s%s", file_name, "_loaded");
	if (-1 == fsop->create_file(&param)){
	    printf ("Error:Can't create ramfs disk");
	    return -1;
	}
	return copy_image(file_name);
}
