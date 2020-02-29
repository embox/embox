/**
 * @file
 * @brief tail-util
 *
 * @date 24.02.20
 * @author Nishchay Agrawal
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define SIZE 200

#define ERROR_MSG -1
#define CMD_FAIL -2

static int read_last_n_lines(FILE *fp, int n) {
	int lines_checked = 0;
	unsigned long long last_line_ptr;
	char buf_str[SIZE];

	if (fseek(fp, 0, SEEK_END)) {
		printf("fseek() failed");
		return ERROR_MSG;
	}
	last_line_ptr = ftell(fp);

	while (last_line_ptr) {
		if (!fseek(fp, --last_line_ptr, SEEK_SET)) {
			if (fgetc(fp) == '\n') {
				if (lines_checked++ == n) {
					break;
				}
			}
		} else {
			perror("fseek() failed");
			return ERROR_MSG;
		}
	}
	if (--lines_checked != n) {
		last_line_ptr = ftell(fp);
		if (fseek(fp, --last_line_ptr, SEEK_SET)) {
			printf("fseek() failed");
			return ERROR_MSG;
		}
	}
	while (fgets(buf_str, sizeof(buf_str), fp)) {
		printf("%s", buf_str);
	}
	return 0;
}

int main(int argc, char **argv) {
	FILE *fp;
	int opt, opt_num = 0, lines = 10, k, temp, found_n = 0;
	int digit_checker = 1;

	if (argc < 2) {
		printf("Usage: more [FILE]\n");
		return 0;
	} else if (argc > 4) {
		printf("Usage: tail -n [digit] [file]\n");
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "n"))) {
		opt_num++;
		switch (opt) {
		case 'n':
			found_n = 1;
			temp = opt_num + 1;
			for (k = 0; k < strlen(argv[temp]); k++) {
				if (!isdigit(argv[temp][k])) {
					digit_checker = 0;
				}
			}

			if (digit_checker == 0) {
				printf("Usage: tail -n [digit] [file]\n");
				return 0;
			}

			lines = atoi(argv[temp]);
			break;

		default:
			printf("Error: No such option.\n");
			return 0;
		}
	}

	if (!found_n && argc > 2) {
		printf("Usage: tail -n [digit] [file]\n");
		return 0;
	}

	if (NULL == (fp = fopen(argv[argc - 1], "r"))) {
		printf("Can't open this file!\n");
		return 0;
	}

	if (read_last_n_lines(fp, lines) < 0) {
		fclose(fp);
		return CMD_FAIL;
	}

	fclose(fp);

	return 0;
}
