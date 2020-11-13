/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <wctype.h>

int wcsncasecmp(const wchar_t *ws1, const wchar_t *ws2, size_t n) {
	wchar_t c1, c2;

	if (n == 0) {
		return 0;
	}
	for (; *ws1; ws1++, ws2++) {
		c1 = towlower(*ws1);
		c2 = towlower(*ws2);
		if (c1 != c2) {
			return ((int) c1 - c2);
		}
		if (--n == 0) {
			return 0;
		}
	}
	return (-*ws2);
}
