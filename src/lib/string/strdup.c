/**
 * @file
 * @brief Implementation of #strdup() function.
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <stdlib.h>

char *strdup(const char *s) {
	char *new;

	new = malloc(strlen(s));
	strcpy(new, s);

	return new;
}
