/**
 * @file
 *
 * @date Apr 10, 2021
 * @author Anton Bondarev
 */

#include <stdio.h>

int vfscanf(FILE */*restrict*/ stream, const char */*restrict*/ format,
       va_list arg) {
	return 0;
}
int vscanf(const char */*restrict*/ format, va_list arg) {
	return 0;
}
int vsscanf(const char */*restrict*/ s, const char */*restrict*/ format,
       va_list arg) {
	return 0;
}
