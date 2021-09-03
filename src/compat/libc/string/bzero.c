/**
 * @file
 *
 * @date Sep 3, 2021
 * @author Anton Bondarev
 */
#include <string.h>
#include <strings.h>

void bzero(void *s, size_t n) {
	memset(s, 0, n);
}
