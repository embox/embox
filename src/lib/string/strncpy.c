/**
 * @file
 * @brief Implementation of #strncpy() function.
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

char *strncpy(char *dst, const char *src, size_t n) {
	char *ret = dst;
#if 1
	while (n && (*dst++ = *src++)) {
		n--;
	}
	while (n--) {
		*dst++ = '\0';
	}
#else /* Duff's device optimization */
	size_t count = (n + 7) / 8;
	switch (n % 8) {
	case 0: do { *dst = *src++;
	case 7:      *dst = *src++;
	case 6:      *dst = *src++;
	case 5:      *dst = *src++;
	case 4:      *dst = *src++;
	case 3:      *dst = *src++;
	case 2:      *dst = *src++;
	case 1:      *dst = *src++;
		} while (--count > 0);
	}
#endif
	return ret;
}
