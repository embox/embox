/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Alexander Kalmuk
 */

#include <wchar.h>

wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n) {
	wchar_t *ret = dst;

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
