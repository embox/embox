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
#include <kernel/prom_printf.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: cat [OPTION]... [FILE]...\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int number = 0, line = 0, new_line = 1;
	FILE *fd;
	char buff;

	if(argc < 2) {
		printf("Please enter correct file name\n");
		return 0;
	}
	getopt_init();

	while (-1 != (opt = getopt(argc - 1, argv, "nh"))) {
		switch (opt) {
		case 'n':
			number = 1;
			break;
		case '?':
			printf("Invalid option `-%c'\n", optopt);
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return 0;
		default:
			return -1;
		}
	}

	if (argc < 2) {
		print_usage();
		return 0;
	}
	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return -1;
	}

	while (fread(&buff, 1, 1, fd) > 0) {
		if (new_line && number) {
			prom_printf("\t%d %c", line++, buff);
		} else {
			prom_printf("%c", buff);
		}
		new_line = (buff == '\n') ? 1 : 0;
	}
	fclose(fd);
	return 0;
}
