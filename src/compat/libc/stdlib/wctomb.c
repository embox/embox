/**
 * @file
 * @brief wctomb function.
 *
 * @see stdlib.h
 *
 * @date 07.12.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>

int wctomb(char *out,  const wchar_t *in) {
	if (in == NULL || out == NULL) {
		return 0;
	}

	*out++ = *in++;

	return sizeof(char);
}
