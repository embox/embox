/**
 * \file ldimg.c
 *
 * \date 03.07.2009
 * \author kse
 */
#include "types.h"
#include "conio.h"
#include "shell.h"
#include "ldimg.h"

static char ldimg_keys[] = {
#include "ldimg_keys.inc"
		};

static void show_help() {
	printf(
#include "ldimg_help.inc"
	);
}

#define ENTRY_PTR 0x40000000


void copy_image()
{
    extern char _piggy_start, _piggy_end, _data_start;

    char *src = &_piggy_start;
    char *dst = (char*)ENTRY_PTR;

    if (&_piggy_start == &_piggy_end) {
        printf("No any images for ldimg in this build.\n");
        return;
    }


    printf("\nCopy piggy image\n");
    printf("from %08X to %08X size %d bytes\n", (unsigned)src, (unsigned)dst, (unsigned)(&_piggy_end) - (unsigned)(&_piggy_start));
    while (src < &_piggy_end) {
        *dst++ = *src++;
    }
}


int ldimg_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;
	int i;
	SHELL_HANDLER_DESCR *shell_handlers;

	keys_amount = parse_arg("ldimg", argsc, argsv, ldimg_keys, sizeof(ldimg_keys),
			keys);

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	copy_image();

	return 0;
}
