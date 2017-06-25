/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.07.2014
 */

#include <string.h>

char *strchrnul(const char *str, int ch) {
	char c = (char) ch;

	while (*str && *str != c) {
		++str;
	}

	return (char *) str;
}

