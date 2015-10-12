/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <fcntl.h>
#include <sys/types.h>

int creat(const char *pathname, mode_t mode) {
	int rc;

	rc = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);

	return rc;
}
