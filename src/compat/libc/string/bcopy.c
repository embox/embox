/**
 * @file
 *
 * @date Sep 3, 2021
 * @author Anton Bondarev
 */
#include <string.h>
#include <strings.h>

void bcopy(const void *src, void *dest, size_t n) {
	memmove(dest, src, n);
}
