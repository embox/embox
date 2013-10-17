/**
 * @file
 * @brief Implementation of #strdup() function.
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <stdlib.h>
#include <string.h>

char * strdup(const char *s) {
	char *ret;

	ret = malloc(strlen(s) + 1);
	if (ret == NULL) {
		return NULL;
	}

	strcpy(ret, s);

	return ret;
}
