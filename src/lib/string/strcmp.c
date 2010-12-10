/**
 * @file
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */
#include <string.h>

int strcmp(const char *str1, const char *str2) {
	for (; *str1 == *str2; ++str1, ++str2) {
		if (*str1 == 0) {
			return 0;
		}
	}
	return *(const unsigned char *) str1 - *(const unsigned char *) str2;
}
