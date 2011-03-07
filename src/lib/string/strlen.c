/**
 * @file
 * @brief Implementation of #strlen() function.
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

size_t strlen(const char * str) {
	const char *eos = str;

	while (*eos++)
		;

	return (int) (eos - str - 1);
}
