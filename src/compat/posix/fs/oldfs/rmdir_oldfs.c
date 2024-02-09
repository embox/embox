/**
 * @file
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <unistd.h>

#include <fs/kfsop.h>

int rmdir(const char *pathname) {
	int rc;

	rc = krmdir(pathname);

	return rc;
}
