/**
 * @file
 * @brief Hack to define link function without -include option
 *
 * @author  Alexander Kalmuk
 * @date    2.07.2013
 */

#ifndef UNISTD_H_
#define UNISTD_H_

#include_next <unistd.h>

static inline int link(const char *oldpath, const char *newpath) {
	printf(">>> link, oldpath - %s, newpath - %s\n", oldpath, newpath);
	return -1;
}

#endif /* UNISTD_H_ */
