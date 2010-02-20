/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

int strlen(const char * str) {
	const char *eos = str;

	while (*eos++)
		;

	return (int) (eos - str - 1);
}
