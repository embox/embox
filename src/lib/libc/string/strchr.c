/**
 * @file
 * @brief Implementation of #strchr() function.
 *
 * @date 1.10.09
 * @author Nikolay Korotky
 *         - Initial import
 * @author Eldar Abusalimov
 *         - Replacing with more simple and obvious implementation
 */

#include <string.h>

char *strchr(const char *str, int ch) {
	char c = (char) ch;

	if (!c) {
		return (char *) str + strlen(str);
	}

	while (*str && *str != c) {
		++str;
	}

	return (*str == c) ? (char *) str : NULL;
}
