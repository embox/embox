/**
 * @file
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: version [-h]\n");
}

static int exec(int argsc, char **argsv) {
#ifdef CONFIG_SVN_REV
	unsigned int rev = CONFIG_SVN_REV;
#endif
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "h");
		switch(nextOption) {
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	printf("                ____\n");
	printf("               |  _ \\\n");
	printf("  ___ _ __ ___ | |_) | _____  __\n");
	printf(" / _ \\ '_ ` _ \\|  _ < / _ \\ \\/ /\n");
	printf("|  __/ | | | | | |_) | (_) >  <  \n");
	printf(" \\___|_| |_| |_|____/ \\___/_/\\_\\\n");
	printf("Date: %12s\n", __DATE__);
	printf("Time: %9s\n", __TIME__);
	printf("Compiler: %s\n", __VERSION__);
#ifdef CONFIG_SVN_REV
	printf("Revision: r%d\n", rev);
#endif
	return 0;
}
