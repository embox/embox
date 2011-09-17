/**
 * @file
 * @brief Implements readline library
 *
 * @date 13.11.10
 * @author Anton Bondarev
 */

#include <lib/readline.h>
#include <drivers/tty.h>

static char nil_string[] = "";

static char *readed_line = nil_string;

static int readed_line_pos = 0;

int readline_getchar(void) {
	if (readed_line[readed_line_pos] == 0) {
		readed_line = readline(nil_string);
		readed_line_pos = 0;
	}
	return (int) readed_line[readed_line_pos++];
}


char *readline(const char *promt) {
	if (NULL == cur_tty) {
		return NULL;
	}
	printf("%s", promt);
	return (char *) tty_readline(cur_tty);
}

void freeline(char* line) {
	if (NULL == cur_tty) {
		return;
	}
	tty_freeline(cur_tty, (uint8_t *) line);
}
