/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <types.h>
#include <sys/stat.h>
#include <fs/kfsop.h>

int kcreat(struct node *node, const char *pathname, mode_t mode) {
	return -1;
}

int kmkdir(struct node *node, const char *pathname, mode_t mode) {
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


int klstat(const char *path, stat_t *buf) {
	return -1;
}

int kmount(char *dev, char *dir, char *fs_type) {
	return -1;
}

int kformat(const char *pathname, const char *fs_type) {
	return -1;
}
