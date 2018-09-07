/**
 * @file
 * @brief head-util
 *
 * @date 07.09.18
 * @author Filipp Chubukov
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

static void screen(FILE *fp, int max_lines) {
	int lines_counter = 0, symbol;

	while (lines_counter < max_lines) {
		symbol = getc(fp);
		switch ((int) symbol) {
		case EOF:
			return;

		case '\n':
			lines_counter++;
			break;
		}

		printf("%c", symbol);
	}
	return;
}

int main(int argc, char **argv) {
	FILE *fp;
	int opt, option_number = 0, lines = 10, k;
	int digit_checker = 1;	

	if (argc < 2) {
		printf ("Usage: more [FILE]\n");
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "n"))) {
		option_number++;
		switch (opt) {
		case 'n':
			for (k = 0; k < strlen(argv[option_number + 1]); k++) {
				if (!isdigit(argv[option_number + 1][k])) digit_checker = 0;
			}

			if (digit_checker == 0) {
				printf("Usage: head -n [digit] [file]\n");
				return 0;
			}

			lines = atoi(argv[option_number + 1]);
			break;

		default:
			printf("Error: No such option.\n");
			return 0;
		}
	
	}

	if (NULL == (fp = fopen(argv[argc - 1], "r"))) {
		printf ("Can't open this file!\n");
		return 0;
	}

	screen(fp, lines);

	fclose(fp);

	return 0;
}
