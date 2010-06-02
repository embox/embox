/**
 * @file
 * @brief boot application image from memory.
 *
 * @date 01.06.10
 * @author Nikolay Korotky
 */

#include <shell_command.h>
#include "uimage.h"

#define COMMAND_NAME     "boot"
#define COMMAND_DESC_MSG "boot application image from memory"
#define HELP_MSG         "Usage: boot [-f format] [-i] -a addr"

static const char *man_page =
    #include "boot_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int uimage_info(unsigned int addr) {
    image_header_t *hdr = (image_header_t *)addr;
    printf("## Checking Image at 0x%08X ...\n", addr);
    printf("Image Header Magic Number: 0x%08X\n", hdr->ih_magic);
    printf("Image Header CRC Checksum: 0x%08X\n", hdr->ih_hcrc);
    printf("Image Creation Timestamp: 0x%08X\n", hdr->ih_time);
    printf("Image Data Size: 0x%08X\n", hdr->ih_size);
    printf("Data  Load  Address: 0x%08X\n", hdr->ih_load);
    printf("Entry Point Address: 0x%08X\n", hdr->ih_ep);
    printf("Image Data CRC Checksum: 0x%08X\n", hdr->ih_dcrc);
    printf("Operating System: 0x%08X\n", hdr->ih_os);
    printf("CPU architecture: 0x%08X\n", hdr->ih_arch);
    printf("Image Type: 0x%08X\n", hdr->ih_type);
    printf("Compression Type: 0x%08X\n", hdr->ih_comp);
    printf("Image Name: %s\n", hdr->ih_name);
    return 0;
}

static int exec(int argsc, char **argsv) {
    int nextOption, info;
    char format;
    unsigned int addr;
    getopt_init();
    do {
        nextOption = getopt(argsc - 1, argsv, "f:a:ih");
        switch(nextOption) {
            case 'f':
        	if (1 != sscanf(optarg, "%c", &format)) {
        	    LOG_ERROR("wrong format %d\n", optarg);
            	    return -1;
            	}
            	break;
            case 'a':
            	if ((optarg == NULL) || (*optarg == '\0')) {
            	    LOG_ERROR("missed address value\n");
            	    return -1;
            	}
            	if (sscanf(optarg, "0x%x", &addr) < 0) {
            	    LOG_ERROR("invalid value \"%s\".\nthe number expected.\n", optarg);
            	    return -1;
            	}
            	break;
            case 'i':
        	info = 1;
            	break;
            case 'h':
            	show_help();
            	return 0;
            case -1:
                break;
            default:
                return 0;
            }
    } while(-1 != nextOption);

    if (info) {
	uimage_info(addr);
    } else {
	//TODO:
    }
    return 0;
}
