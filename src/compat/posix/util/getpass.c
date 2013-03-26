/**
 * @file
 * @brief
 *
 * @date 19.03.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

char * getpass(const char *prompt) {
	static char pass[PASS_MAX + 2]; /* for \n\0 */
	size_t pass_len;
    struct termios t;

	/* print prompt */
	if (0 > fprintf(stdout, "%s", prompt)) {
		return NULL;
	}

	/* read password from stdin (with disabling ECHO-mode) */
	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return NULL;
	}

	t.c_lflag &= ~ECHO;

	if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &t)) {
		return NULL;
	}

	t.c_lflag |= ECHO;

	if (NULL == fgets(&pass[0], sizeof pass, stdin)) {
		tcsetattr(STDIN_FILENO, TCSANOW, &t);
		return NULL;
	}

	if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &t)) {
		return NULL;
	}

	/* remove newline character */
	pass_len = strlen(&pass[0]);

	if (pass[pass_len - 1] != '\n') {
		return NULL; /* error: no newline at the end of line.
						password is too long or EOF is encountered */
	}

	pass[pass_len - 1] = '\0';

	/* done */
	return &pass[0];
}
