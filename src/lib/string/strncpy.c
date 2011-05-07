/**
 * @file
 * @brief Implementation of #strncpy() function.
 *
 * @date 20.11.09
 * @author Eldar Abusalimov
 */

#include <string.h>

char *strncpy(char *dst, const char *src, size_t n) {
	char *ret = dst;

	do {
		if (!n--) {
			return ret;
		}
	} while ((*dst++ = *src++));

	while (n--) {
		*dst++ = '\0';
	}

	return ret;
}
