/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */


#include <wchar.h>
#include <inttypes.h>
#include <string.h>

int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n) {
	wchar_t c1;
	wchar_t c2;

	while (n >= 4) {
		c1 = s1[0];
		c2 = s2[0];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		c1 = s1[1];
		c2 = s2[1];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		c1 = s1[2];
		c2 = s2[2];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		c1 = s1[3];
		c2 = s2[3];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		s1 += 4;
		s2 += 4;
		n -= 4;
	}
	if (n > 0) {
		c1 = s1[0];
		c2 = s2[0];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		++s1;
		++s2;
		--n;
	}
	if (n > 0) {
		c1 = s1[0];
		c2 = s2[0];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
		++s1;
		++s2;
		--n;
	}
	if (n > 0) {
		c1 = s1[0];
		c2 = s2[0];
		if (c1 - c2 != 0) {
			return c1 > c2 ? 1 : -1;
		}
	}
	return 0;
}
