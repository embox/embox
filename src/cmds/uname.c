/*
 * uname.c
 *
 *  Created on: Oct 1, 2012
 *      Author: Sergey
 */


#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: uname [-a]\n");
}

static int exec(int argc, char **argv) {
#ifdef CONFIG_SVN_REV
	unsigned int rev = CONFIG_SVN_REV;
#endif
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a"))) {
		switch(opt) {
		case 'a':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}
	printf("OS: Embox");
    printf("\n");
	printf("Date: %12s\n", __DATE__);
	printf("Time: %9s\n", __TIME__);
	printf("Compiler: %s\n", __VERSION__);
#ifdef CONFIG_SVN_REV
	printf("Revision: r%d\n", rev);
#endif
	return 0;
}
