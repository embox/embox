/**
 * @file
 *
 * @data 12 oct. 2015
 * @author: Anton Bondarev
 */

#include <sys/stat.h>

#include <fs/kfsop.h>

int lstat(const char *path, struct stat *buf) {
	int rc;

	rc = klstat(path, buf);
	DPRINTF(("lstat(%s) = %d\n", path, rc));
	return rc;
}

int stat(const char *path, struct stat *buf) {
	int rc;

	rc = lstat(path, buf);
	DPRINTF(("stat(%s) = %d\n", path, rc));
	return rc;
}
