/**
 * @file
 * @brief wc-util
 *
 * @date 19.07.18
 * @author Filipp Chubukov
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void wc_util(FILE *file_in, int *lines_in_file, int *words_in_file,
    int *bytes_in_file, int *max_line_in_file) {
	int byte_walker, prev_byte = 0;
	int count_len = 0;
	bool prev_symbol = false;
	bool next_symbol = false;

	int lines = 0;
	int words = 0;
	int bytes = 0;
	int max_line = 0;

	while ((byte_walker = fgetc(file_in)) != EOF) {
		prev_byte = byte_walker;

		bytes++;

		if (byte_walker == '\n') {
			lines++;

			if (max_line < count_len) {
				max_line = count_len;
			}

			count_len = 0;
		}
		else {
			if (byte_walker == '\t') {
				count_len += 8;
			}
			else {
				count_len++;
			}
		}

		next_symbol = !isspace(byte_walker);

		if (next_symbol) {
			prev_symbol = true;
		}

		if (prev_symbol && !next_symbol) {
			prev_symbol = false;
			words++;
		}
	}

	if (prev_byte != '\n' && prev_byte != EOF) {
		lines++;
		if (prev_symbol) {
			words++;
		}
	}

	*lines_in_file = lines;
	*words_in_file = words;
	*bytes_in_file = bytes;
	*max_line_in_file = max_line;
}

int main(int argc, char **argv) {
	int opt, j, param_number = 0;

	bool print_lines, print_words, print_bytes;
	bool print_linelength;

	int lines_count = 0;
	int words_count = 0;
	int bytes_count = 0;
	int max_line = 0;

	print_lines = print_words = print_bytes = false;
	print_linelength = false;

	if (argc < 2) {
		printf("No file name given.\n");
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "lLcw"))) {
		param_number++;
		switch (opt) {
		case 'l':
			print_lines = true;
			break;

		case 'L':
			print_linelength = true;
			break;

		case 'c':
			print_bytes = true;
			break;

		case 'w':
			print_words = true;
			break;

		default:
			printf("Error: No such option.\n");
			return 0;
		}
	}

	if (!(print_lines || print_words || print_bytes || print_linelength)) {
		print_lines = print_words = print_bytes = true;
	}

	for (j = param_number + 1; j < argc; j++) {
		int lines_in_file;
		int words_in_file;
		int bytes_in_file;
		int max_line_in_file;

		FILE *file_in;
		file_in = fopen(argv[j], "r");

		if (!file_in) {
			printf("Error with opening file: %s\n", argv[j]);
			return 0;
		}

		wc_util(file_in, &lines_in_file, &words_in_file, &bytes_in_file,
		    &max_line_in_file);

		if (print_lines) {
			printf("%d  ", lines_in_file);
			lines_count = lines_count + lines_in_file;
		}

		if (print_words) {
			printf("%d  ", words_in_file);
			words_count = words_count + words_in_file;
		}

		if (print_bytes) {
			printf("%d  ", bytes_in_file);
			bytes_count = bytes_count + bytes_in_file;
		}

		if (print_linelength) {
			printf("%d  ", max_line_in_file);
			if (max_line < max_line_in_file) {
				max_line = max_line_in_file;
			}
		}
		fclose(file_in);
		printf("%s \n", argv[j]);
	}

	if (argc - param_number - 1 > 1) {
		if (print_lines) {
			printf("%d  ", lines_count);
		}
		if (print_words) {
			printf("%d  ", words_count);
		}
		if (print_bytes) {
			printf("%d  ", bytes_count);
		}
		if (print_linelength) {
			printf("%d  ", max_line);
		}

		printf("total\n");
	}

	return 0;
}
