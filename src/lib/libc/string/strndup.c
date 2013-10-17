/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */
#include <stdlib.h>

#include <util/math.h>

#include <string.h>

char *strndup(const char *s, size_t size) {
	char *ret;
	size_t len = min(strlen(s) + 1, size);

	ret = malloc(len);
	if (ret == NULL) {
		return NULL;
	}

	strncpy(ret, s, len);

	return ret;
}
