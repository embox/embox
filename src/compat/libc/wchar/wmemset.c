/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

wchar_t* wmemset(wchar_t *wcs, wchar_t wc, size_t n) {
	wchar_t *wp = wcs;

	while (n >= 4) {
		wp[0] = wc;
		wp[1] = wc;
		wp[2] = wc;
		wp[3] = wc;
		wp += 4;
		n -= 4;
	}
	if (n > 0) {
		wp[0] = wc;
		if (n > 1) {
			wp[1] = wc;
			if (n > 2)
				wp[2] = wc;
		}
	}
	return wcs;
}
