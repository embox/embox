/**
 * @file
 *
 * @date Oct 28, 2020
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <locale.h>

char *setlocale(int category, const char *locale) {
	(void)category; (void)locale;
	return NULL;
}

struct lconv *localeconv(void) {
	return NULL;
}
