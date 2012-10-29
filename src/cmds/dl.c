/**
 * @file
 * @brief Loadreloc loads relocatable ELF binaries.
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdio.h>

#include <lib/libelf.h>
#include <lib/libdl.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

EMBOX_CMD(exec);

extern int __link_elf(void);

static int exec(int argc, char **argv) {
	return __link_elf();
}
