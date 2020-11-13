/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

wchar_t* wmemchr(const wchar_t *s, wchar_t c, size_t n) {
	/* For performance reasons unfold the loop four times.  */
	while (n >= 4) {
		if (s[0] == c) {
			return (wchar_t*) s;
		}
		if (s[1] == c) {
			return (wchar_t*) &s[1];
		}
		if (s[2] == c) {
			return (wchar_t*) &s[2];
		}
		if (s[3] == c) {
			return (wchar_t*) &s[3];
		}
		s += 4;
		n -= 4;
	}
	if (n > 0) {
		if (*s == c) {
			return (wchar_t*) s;
		}
		++s;
		--n;
	}
	if (n > 0) {
		if (*s == c) {
			return (wchar_t*) s;
		}
		++s;
		--n;
	}
	if (n > 0) {
		if (*s == c) {
			return (wchar_t*) s;
		}
	}
	return NULL;
}
