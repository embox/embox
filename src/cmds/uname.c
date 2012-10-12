/**
 * @file
 *
 * @date Oct 1, 2012
 * @author Sergey Ivantsov
 */


#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <revision.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: uname [OPTION]...\n");
	printf("Print certain system information\n\n");
	printf("\t-a\tprint all information\n");
	printf("\t-s\tprint the kernel name\n");
	printf("\t-r\tprint the kernel release\n");
}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hars"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'a':
			printf("Embox %s %12s %9s", EMBOX_REVISION, __DATE__, __TIME__);
			return 0;
		case 'r':
			printf("%s", EMBOX_REVISION);
			return 0;
		case 's':
			printf("Embox");
			return 0;
		default:
			return 0;
		}
	}
	printf("Usage: uname [-hars]\n");

	return 0;
}
