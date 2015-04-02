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

/**
 * @brief Non posix extension of deprecated @a getpass that not use static buffers and
 * therefore thread-safe.
 *
 * @param prompt Prompt to be printed to user
 * @param buf Buffer to store user's input
 * @param buflen @a buf length
 *
 * @return
 * 	NULL on error
 * 	pointer to user entered password
 */
char *getpass_r(const char *prompt, char *buf, size_t buflen) {
	size_t pass_len;
	struct termios t;
	/* print prompt */
	if (0 > fprintf(stdout, "%s", prompt)) {
		return NULL;
	}
	fflush(stdout);

	/* read password from stdin (with disabling ECHO-mode) */
	if (-1 == tcgetattr(STDIN_FILENO, &t)) {
		return NULL;
	}

	t.c_lflag &= ~ECHO;

	if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &t)) {
		return NULL;
	}

	t.c_lflag |= ECHO;

	if (NULL == fgets(buf, buflen, stdin)) {
		tcsetattr(STDIN_FILENO, TCSANOW, &t);
		return NULL;
	}

	if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &t)) {
		return NULL;
	}

	/* remove newline character */
	pass_len = strlen(buf);

	if (buf[pass_len - 1] != '\n') {
		return NULL; /* error: no newline at the end of line.
						password is too long or EOF is encountered */
	}

	buf[pass_len - 1] = '\0';

	/* done */
	return buf;
}

char * getpass(const char *prompt) {
	static char pass[PASS_MAX + 2]; /* for \n\0 */
	return getpass_r(prompt, pass, sizeof(pass));
}
