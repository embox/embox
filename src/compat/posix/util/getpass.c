/**
 * @file
 * @brief
 *
 * @date 19.03.13
 * @author Ilia Vaprol
 */

#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

char * getpass(const char *prompt) {
	static char pass[PASS_MAX + 1];

	fprintf(stdout, "%s", prompt);

	return fgets(&pass[0], PASS_MAX, stdin);
}
