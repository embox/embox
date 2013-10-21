/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Alexander Kalmuk
 */

#include <wchar.h>

int wcscmp(const wchar_t *s1, const wchar_t *s2) {

	while (*s1 && *s1 == *s2) {
		++s1;
		++s2;
	}

	return *s1 - *s2;
}

