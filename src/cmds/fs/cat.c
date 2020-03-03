/**
 * @file
 * @brief Concatenate files to standard output.
 *
 * @date 17.03.10
 * @author Nikolay Korotky
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
	int no_of_arg;
	char *file_names[argc];
	size_t arg_len;

	for (no_of_arg = 0; no_of_arg < argc; no_of_arg++) {
		arg_len = strlen(argv[no_of_arg]);
		file_names[no_of_arg] = (char *)malloc (sizeof(char) * arg_len);
		strcpy (file_names[no_of_arg], argv[no_of_arg]);
	}

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "nh"))) {
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

	for (no_of_arg = 1; no_of_arg < argc; no_of_arg++) {
		if (strcmp ("-n", file_names[no_of_arg]) != 0) {
			if (NULL == (fd = fopen(file_names[no_of_arg], "r"))) {
				printf("Can't open file %s\n", file_names[no_of_arg]);
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
		}
	}

	for (no_of_arg = 0; no_of_arg < argc; no_of_arg++) {
		char *file_name = file_names[no_of_arg];
		free (file_name);
	}

	return 0;
}