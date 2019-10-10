/**
 * @file
 * @brief utime function
 *
 * @author  Filipp Chubukov
 * @date    16.04.2014
 */

#include <utime.h>
#include <fs/kfsop.h>
#include <errno.h>

int utime(const char *path, const struct utimbuf *times) {
	int res;
	if (0 != (res = kutime(path, times))) {
		errno = -res;
		return -1;
	}
	return 0;	
}
