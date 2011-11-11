/**
 * @file
 * @brief Implementation of #strupr() function.
 *
 * @date 06.08.2011
 * @author Gerald Hoch
 */

#include <string.h>

char *strupr(char *string) {
	char *cp;

	for (cp = string; *cp; ++cp) {
		if ('a' <= *cp && *cp <= 'z')
			*cp += 'A' - 'a';
	}

	return(string);
}
