/**
 * @file
 * @brief mblen function.
 *
 * @see stdlib.h
 *
 * @date 01.11.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>
#include <stdio.h>
int mblen(char *str, size_t max) {
	char *tmp = str;
	char *buffer = tmp;
	int sum = 0;
	int i =0;
	while (buffer) {
		if (i >= max) {
			return sum;
		}
		sum = sum + (int)sizeof(tmp);
	
		*buffer = *tmp++;
		i++;
	}
	
	return sum;
}
