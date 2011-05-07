/**
 * @file
 * @brief Implementation of #memset() function.
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

void *memset(void *p, int c, size_t n) {
	char *pb = (char *) p;
	char *pbend = pb + n;
	while (pb != pbend) {
		*pb++ = c;
	}
	return p;
}
