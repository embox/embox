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

size_t mbstowcs(wchar_t *wcstring, const char *mbstring, size_t max) {
	int i = 0;
	if (mbstring == NULL || wcstring == NULL) {
		return 0;
	}

	while (*mbstring != '\0') {
		if (i >= max) {
			return i;
		}
		mbtowc(wcstring, mbstring, sizeof(char));
		wcstring++;
		mbstring++;	
		i++;
	}
	mbtowc(wcstring, mbstring, sizeof(char)); /* Сonvert '\0'*/

	return i;
}
