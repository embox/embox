/**
 * @file
 *
 * @date Oct 17, 2013
 * @author: Anton Bondarev
 */

#include <ctype.h>

long atol(const char *nptr) {
	int c;
	int sign;
	long total;
	const unsigned char *p = (const unsigned char *) nptr;

	while (isspace(*p))
		++p;

	c = *p++;
	sign = c;
	if (c == '-' || c == '+')
		c = *p++;

	total = 0;
	while (isdigit(c)) {
		total = 10 * total + (c - '0');
		c = *p++;
	}

	if (sign == '-') {
		return -total;
	} else {
		return total;
	}
}

int atoi(const char *nptr) {
	return (int) atol(nptr);
}
