/**
 * @file
 * @brief wcstombs function.
 *
 * @see stdlib.h
 *
 * @date 01.11.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>

size_t wcstombs(char *mbstr,  const wchar_t *wcstr, size_t max) {
	int i = 0;
	if (wcstr == NULL || mbstr == NULL) {
		return 0;
	}

	while (*wcstr != L'\0') {
		if (i >= max) {
			return i;
		}
	
		wctomb(mbstr, wcstr);
		mbstr++;
		wcstr++;
		i++;
	}
	wctomb(mbstr, wcstr); /* Сonvert '\0'*/

	return i;
}
