/**
 * @file
 * @brief Concatenate files to standard output.
 *
 * @date 17.03.10
 * @author Nikolay Korotky
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


static void print_usage(void) {
	printf("Usage: cat [OPTION]... [FILE]...\n");
}

int main(int argc, char **argv) {
	int opt;
	int number = 0, line = 0, new_line = 1;
	FILE *fd;
	char buff;

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

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
			return -EINVAL;
		}
	}

	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return -errno;
	}

	while (fread(&buff, 1, 1, fd) == 1) {
		if (new_line && number) {
			printf("\t%d %c", line++, buff);
		} else {
			printf("%c", buff);
		}
		new_line = (buff == '\n') ? 1 : 0;
	}

	if (buff != '\n') {
		printf("\n");
	}

	fflush(stdout);

	fclose(fd);
	return 0;
}
