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

static int exec(int argc, char **argv) {
	int opt;
	FILE *fd;
	char buff[1] = " ";
	getopt_init();
	do {
		opt = getopt(argc - 1, argv, "h");
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	if (argc < 2) {
		print_usage();
		return 0;
	}
	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
                printf("Can't open file %s\n", argv[argc - 1]);
                return -1;
        }
	while (fread(buff, sizeof(buff), 1, fd) > 0) {
		printf("%s", buff);
	}
	fclose(fd);
	return 0;
}
