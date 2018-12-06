/**
 * @file
 * @brief mbstowcs function.
 *
 * @see stdlib.h
 *
 * @date 01.11.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>
#include <string.h>
#include <defines/size_t.h>
#include <defines/wchar_t.h>

size_t wcstombs(char *mbstr,  wchar_t *wcstr, size_t max) {
	wchar_t *tmp =  wcstr;
	int i = 0;

	while (tmp) {
		if (i >= max) {
			return i;
		}
	
		*mbstr++ = (char) *tmp++;
		i++;
	}
	return i;
}
