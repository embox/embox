/**
 * @file
 * @brief Implementation of #strlwr() function.
 *
 * @date 06.08.2011
 * @author Gerald Hoch
 */

#include <string.h>

char *strlwr (char *string) {
	char *cp;

	for (cp = string; *cp; ++cp) {
		if ('A' <= *cp && *cp <= 'Z')
			*cp += 'a' - 'A';
	}

	return string;
}
