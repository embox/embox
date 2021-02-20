/**
 * @file
 *
 * @date Feb 20, 2021
 * @author Vedant Paranjape
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

size_t wcsnlen(const wchar_t *s, size_t maxlen) {
	size_t len = 0;

	for (len = 0; len < maxlen; len++, s++) {
		if (!*s) {
			break;
		}
	}

	return (len);
}
