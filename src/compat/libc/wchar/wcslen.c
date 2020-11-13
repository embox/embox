/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

size_t wcslen(const wchar_t *s) {
	size_t len = 0;

	while (s[len] != L'\0') {
		if (s[++len] == L'\0') {
			return len;
		}
		if (s[++len] == L'\0') {
			return len;
		}
		if (s[++len] == L'\0') {
			return len;
		}
		++len;
	}
	return len;
}
