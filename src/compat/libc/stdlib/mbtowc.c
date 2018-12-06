/**
 * @file
 * @brief mbtowc function.
 *
 * @see stdlib.h
 *
 * @date 01.11.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>

int mbtowc(wchar_t *out, const char *in, size_t n) {
	if (in == NULL || out == NULL) {
		return 0;
	}
	
	*out = *in;	
	
	return sizeof(char);
}
