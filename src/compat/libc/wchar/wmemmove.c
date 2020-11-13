/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n) {
	return (wchar_t *) memmove ((char *) dest, (char *) src, n * sizeof (wchar_t));
}
