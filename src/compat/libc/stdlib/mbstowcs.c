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

size_t mbstowcs(wchar_t *wcstring, char *mbstring, size_t max) {
	char *tmp =  mbstring;
	int i = 0;

	while (tmp) {
		if (i >= max) {
			return i;
		}
	
		*wcstring++ = (wchar_t)*tmp++;	
		i++;
		
	}
	return i;
}
