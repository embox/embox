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

locale_t newlocale(int category_mask, const char *locale, locale_t base) {
	if (!locale || (*locale == '\0') || !strcmp(locale, "POSIX")
	    || !strcmp(locale, "C")) {
		return localeconv();
	}

	return NULL;
}
