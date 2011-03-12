/**
 * @file
 * @brief Concatenate files to standard output.
 *
 * @date 17.03.10
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: cat [FILES]\n");
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	FILE *fd;
	char buff[1] = " ";
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "h");
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

	if (argsc < 2) {
		print_usage();
		return 0;
	}
	fd = fopen(argsv[argsc - 1], "r");
	while (fread(buff, sizeof(buff), 1, fd) > 0) {
		printf("%s", buff);
	}
	fclose(fd);
	return 0;
}
