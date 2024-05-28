/**
 * @file
 * @brief prints file to standart output screen by screen
 *
 * @date 22.09.2013
 * @author Denis Deryugin
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TAB_SIZE      4
#define SCREEN_HEIGHT 24
#define SCREEN_WIDTH  80

static void screen(FILE *fp) {
	char buff[SCREEN_WIDTH + 1];
	int cmd = 0, columns, lines, x, y, i;

	columns = 80;
	lines = SCREEN_HEIGHT;

	while (1) {
		for (x = 0; x < lines - 1; x++) {
			for (y = 0; y < columns; y++) {
				if (fread(&buff[y], 1, 1, fp) == 1) {
					switch ((int)buff[y]) {
					case '\n':
						/*	End of the line, filling the rest of buffer */
						memset(buff + y, ' ', columns - y);
						y = columns;
						break;

					case '\t':
						/*	Perform tab instert	*/
						memset(buff + y, ' ', TAB_SIZE);
						y += TAB_SIZE - 1;
						break;
					}
				}
				else {
					for (i = 0; i < y; i++) {
						printf("%c", buff[i]);
					}

					return;
				}
			}
			/*	In case if we got out of the actual line size	*/
			buff[columns - 1] = '\n';
			for (i = 0; i < columns; i++) {
				printf("%c", buff[i]);
			}
		}

		printf("---More---\n");
		cmd = getchar();

		if (cmd == 'q') {
			return;
		}
	}
}

int main(int argc, char **argv) {
	FILE *fp;

	if (argc < 2) {
		printf("Usage: more [FILE]\n");
		return 0;
	}

	if (NULL == (fp = fopen(argv[1], "r"))) {
		printf("Can't open this file!\n");
		return 0;
	}

	screen(fp);

	fclose(fp);

	return 0;
}
