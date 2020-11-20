/**
 * @file
 *
 * @date Nov 20, 2020
 * @author Anton Bondarev
 */

#include <errno.h>
#include <wchar.h>
#include <wctype.h>
#include <inttypes.h>

unsigned long long wcstoull(const wchar_t *restrict nptr,
		wchar_t **restrict endptr, int base) {
	const wchar_t *s;
	unsigned long long acc;
	wchar_t c;
	unsigned long long cutoff;
	int neg, any, cutlim;

	/*
	 * See strtoull for comments as to the logic used.
	 */
	s = nptr;
	do {
		c = *s++;
	} while (iswspace(c));

	if (c == L'-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == L'+') {
			c = *s++;
		}
	}
	if ((base == 0 || base == 16) && c == L'0' && (*s == L'x' || *s == L'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = c == L'0' ? 8 : 10;
	}
	acc = any = 0;
	if (base < 2 || base > 36) {
		goto noconv;
	}

	cutoff = ULLONG_MAX / base;
	cutlim = ULLONG_MAX % base;
	for (;; c = *s++) {
		if (c >= L'0' && c <= L'9') {
			c -= L'0';
		} else if (c >= L'A' && c <= L'Z') {
			c -= L'A' - 10;
		} else if (c >= L'a' && c <= L'z') {
			c -= L'a' - 10;
		} else {
			break;
		}
		if (c >= base) {
			break;
		}
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULLONG_MAX;
		errno = ERANGE;
	} else if (!any) {
		noconv: errno = EINVAL;
	} else if (neg) {
		acc = -acc;
	}
	if (endptr != NULL) {
		*endptr = (wchar_t*) (any ? s - 1 : nptr);
	}
	return acc;
}
