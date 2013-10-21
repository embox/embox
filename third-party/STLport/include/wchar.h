/*
 * wchar.h
 *
 *  Created on: 2013-sep-25
 *      Author: fsulima
 */

#ifndef WCHAR_H_
#define WCHAR_H_

static inline
int wcscmp(const wchar_t *s1, const wchar_t *s2) {

	while (*s1 && *s1 == *s2) {
		++s1;
		++s2;
	}

	return *s1 - *s2;
}

static inline
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

#endif /* WCHAR_H_ */
