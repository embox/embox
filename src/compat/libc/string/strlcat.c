/**
 * @file
 * @brief
 *
 * @date 02.12.2025
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <string.h>

size_t strlcat(char *dst, const char *src, size_t size) {
	const char *d = dst;
	const char *s = src;
	size_t n = size;
	size_t len;

	while ((n-- != 0) && (*dst != '\0')) {
		dst++;
	}

	len = dst - d;
	n = size - len;

	if (n-- == 0) {
		return (len + strlen(src));
	}

	while (*src != '\0') {
		if (n != 0) {
			*dst++ = *src;
			n--;
		}
		src++;
	}
	*dst = '\0';

	return len + (src - s);
}
