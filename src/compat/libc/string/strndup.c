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
	size_t len = min(strlen(s), size);

	ret = malloc(len + 1);
	if (ret == NULL) {
		return NULL;
	}
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#endif
	strncpy(ret, s, len);
	ret[len] = '\0';
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic pop
#endif
#endif
	return ret;
}
