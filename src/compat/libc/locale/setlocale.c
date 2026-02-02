/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.01.26
 */

#include <locale.h>
#include <stddef.h>
#include <string.h>

char *setlocale(int category, const char *locale) {
	if (!locale || (*locale == '\0') || !strcmp(locale, "POSIX")
	    || !strcmp(locale, "C")) {
		return "C";
	}

	return NULL;
}
