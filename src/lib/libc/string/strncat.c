/**
 * @file
 * @brief Implementation of #strncat() function.
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

char *strncat(char *s1, const char *s2, size_t n) {
	char c;
	char *s = s1;

	/* Find the end of S1.  */
	do
		c = *s1++;
	while (c != '\0');

	/* Make S1 point before next character, so we can increment
		it while memory is read (wins on pipelined cpus).  */
	s1 -= 2;

	if (n >= 4) {
		size_t n4 = n >> 2;
		do {
			c = *s2++;
			*++s1 = c;
			if (c == '\0')
				return s;
			c = *s2++;
			*++s1 = c;
			if (c == '\0')
				return s;
			c = *s2++;
			*++s1 = c;
			if (c == '\0')
				return s;
			c = *s2++;
			*++s1 = c;
			if (c == '\0')
				return s;
		} while (--n4 > 0);
		n &= 3;
	}

	while (n > 0) {
		c = *s2++;
		*++s1 = c;
		if (c == '\0')
			return s;
		n--;
	}

	if (c != '\0')
		*++s1 = '\0';

	return s;
}
