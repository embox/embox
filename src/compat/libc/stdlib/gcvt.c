#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *gcvt(double number, size_t ndigit, char *buf) {
	size_t len;
	int i;

	if (!buf) {
		return NULL;
	}

	sprintf(buf, "%lf", number);
	len = strlen(buf);

	if (number < 0) {
		ndigit += 1;
	}

	if ((int)number == 0) {
		ndigit += 1;
	}

	for (i = 0; (i < len) && (i < ndigit); i++) {
		if (buf[i] != '.') {
			continue;
		}
		ndigit += 1;
	}

	while (len < ndigit) {
		buf[len++] = '0';
	}

	buf[ndigit] = '\0';

	return buf;
}
