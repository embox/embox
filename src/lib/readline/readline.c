/**
 * @file
 *
 * @brief Implements readline library
 *
 * @date 13.11.2010
 * @author Anton Bondarev
 */
#include <lib/readline.h>
#include <drivers/tty.h>

char *readline(const char *promt) {
	if (NULL == cur_tty) {
		return NULL;
	}
	printf("%s", promt);
	return (char *)tty_readline(cur_tty);
}

void freeline(char* line) {
	if (NULL == cur_tty) {
		return;
	}
	tty_freeline(cur_tty, (uint8_t *)line);
}
