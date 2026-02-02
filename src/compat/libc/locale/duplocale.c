/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.01.26
 */

#include <locale.h>

locale_t duplocale(locale_t locobj) {
	return localeconv();
}
