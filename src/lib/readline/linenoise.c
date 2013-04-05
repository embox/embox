/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#include <errno.h>
#include <lib/linenoise_1.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stddef.h>

#define DEFAULT_HISTORY_FILE "~/.history"


char * readline(const char *prompt) {
	return linenoise(prompt != NULL ? prompt : "");
}

void add_history(const char *line) {
	linenoiseHistoryAdd(line);
}

int read_history(const char *filename) {
	return -1 != linenoiseHistoryLoad(filename != NULL
				? (char *)filename : DEFAULT_HISTORY_FILE)
			? 0 : errno;
}

int write_history(const char *filename) {
	return -1 != linenoiseHistorySave(filename != NULL
				? (char *)filename : DEFAULT_HISTORY_FILE)
			? 0 : errno;
}
