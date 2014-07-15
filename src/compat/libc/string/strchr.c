/**
 * @file
 * @brief Implementation of #strchr() function.
 *
 * @date 1.10.09
 * @author Nikolay Korotky
 *         - Initial import
 * @author Eldar Abusalimov
 *         - Replacing with more simple and obvious implementation
 * @author Anton Kozlov
 *         - Implemented via strchrnul
 */

#include <string.h>

char *strchr(const char *str, int ch) {
	char *chp = strchrnul(str, ch);

	if (*chp == '\0') {
		return ch == *chp ? chp : NULL;
	}

	return chp;
}
