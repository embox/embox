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
	printf("Usage: uname [OPTION]...\n"); //[-h -a -r -s]\n");
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
			printf("Embox 0.3.2 %12s %9s", __DATE__, __TIME__);
			return 0;
		case 'r':
			printf("0.3.2");
			return 0;
		case 's':
			printf("Embox");
			return 0;
		default:
			return 0;
		}
	}
	printf("Usage: uname [-hars]\n");
/*	printf("OS: Embox\n");

	printf("Date: %12s\n", __DATE__);
	printf("Time: %9s\n", __TIME__);
	printf("Compiler: %s\n", __VERSION__);
*/
	return 0;
}
