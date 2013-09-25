/*
 * @file
 * @brief prints file to standart output screen by screen
 *
 * @date 22.09.2013
 * @author Denis Deryugin
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curses.h>

#define TAB_SIZE 4
#define MAX_SCREEN_WIDTH 256

EMBOX_CMD(exec);

static char buff[MAX_SCREEN_WIDTH], info[MAX_SCREEN_WIDTH];

static void screen(FILE *fp) {
    int cmd = 0, columns, lines, x, y;
	WINDOW *std;
	bool endOfFile = false;

	std = initscr();
	columns = std->endx - std->begx;
	if (columns >= MAX_SCREEN_WIDTH - 1) {
		columns = MAX_SCREEN_WIDTH - 1;
	}
	lines	= std->endy - std->begy;
	refresh();
	cbreak();
	noecho();

	while (1){
		if (!endOfFile) {
			for (x = 0; x < lines - 1; x++) {
				for (y = 0; y < columns; y++) {
					buff[y] = !endOfFile ? getc(fp) : ' ';

					switch (buff[y]) {
						case EOF:
							buff[y] = ' ';
							endOfFile = true;
							break;
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
				/*	In case if we got out of the actual line size	*/
				buff[columns] = '\0';

				addstr(buff);
			}
			memset(info, '\0', columns);
			memcpy(info, "---More---", 10);
			memset(buff, ' ', columns);
			if (endOfFile) {
				strcat(info, " End of the file! Press 'q' to quit.");
			}
			memcpy(buff, info, strlen(info));
			attron(A_REVERSE);
			addstr(buff);
			attron(A_NORMAL);
			refresh();
		}

		cmd = getch();
		if (cmd == 'q') {
			endwin();
			return;
		}
	}
	endwin();
	return;
}

static int exec(int argc, char **argv) {
	FILE *fp;

	if (argc < 2) {
		printf ("Usage: more [FILE]\n");
		return 0;
	}

	if (NULL == (fp = fopen(argv[1], "r"))) {
		printf ("Can't open this file!\n");
		return 0;
	}

	screen(fp);

	fclose(fp);
	return 0;
}
