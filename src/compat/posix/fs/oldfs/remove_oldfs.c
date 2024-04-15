/**
 * @file
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>

#include <fs/kfsop.h>

int remove(const char *pathname) {
	int rc;

	rc = kremove(pathname);

	return rc;
}
