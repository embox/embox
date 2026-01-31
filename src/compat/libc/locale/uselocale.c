/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.01.26
 */

#include <locale.h>

locale_t uselocale(locale_t newloc) {
	return localeconv();
}
