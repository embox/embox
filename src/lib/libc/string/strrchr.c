/**
 * @file
 * @brief Implementation of #strrchr() function.
 *
 * @date 01.10.09
 * @author Nikolay Korotky
 */

#include <string.h>

char *strrchr(const char *str, int ch) {
	char c = (char) ch;
	const char *found = NULL;

	if (!c) {
		return (char *) str + strlen(str);
	}

	while ((str = strchr(str, ch))) {
		found = str++;
	}

	return (char *) found;
}
