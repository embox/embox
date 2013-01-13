/**
 * @file
 * @brief
 *
 * @date 8.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_logging.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FIXME Suppose for now that s don't contain '\0' */
void javacall_print_chars(const char* s, int length) {
	char *new_str = malloc(length + 1);

	memcpy(new_str, s, length);
	new_str[length] = '\0';
	printf("%s", new_str);
	free(new_str);
}

void javacall_print(const char *s) {
	printf("%s", s);
}

const char * javacall_jlong_format_specifier() {
	return "%lld";
}

const char * javacall_julong_format_specifier() {
	return "%llu";
}

