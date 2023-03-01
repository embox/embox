/**
 * @file
 * @brief
 * @date 19.05.19
 * @author Alexander Kalmuk
 */

#include <stdlib.h>
#include <util/log.h>

int mkstemp(char *path_template) {
	(void)path_template;
	log_error("");
	return -1;
}

int mkstemps(char *path_template, int suffixlen) {
	(void)path_template;
	(void)suffixlen;
	log_error("");
	return -1;
}
