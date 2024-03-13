/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fs/kfsop.h>

int kcreat(struct path *node, const char *pathname, mode_t mode) {
	return -1;
}

int kmkdir(const char *pathname, mode_t mode) {
	return -1;
}

int kremove(const char *pathname) {
	return -1;
}

int kunlink(const char *pathname) {
	return -1;
}

int krmdir(const char *pathname) {
	return -1;
}

int kmount(const char *source, const char *dest, const char *fs_type) {
	return -1;
}

int kformat(const char *pathname, const char *fs_type) {
	return -1;
}

int kumount(const char *dir) {
	return -1;
}

int kutime(const char *path,const struct utimbuf *times) {
	return -1;
}
