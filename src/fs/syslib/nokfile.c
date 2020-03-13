/**
 * @file
 *
 * @date 27.11.12
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

struct inode;
struct file_desc;
struct stat;

struct idesc *kopen(struct inode *node, int flag) {
	return NULL;
}

ssize_t kwrite(const void *buf, size_t size, struct file_desc *file) {
	return -ENOSYS;
}

ssize_t kread(void *buf, size_t size, struct file_desc *desc) {
	return -ENOSYS;
}


void kclose(struct file_desc *desc) {
}

int kseek(struct file_desc *desc, long int offset, int origin) {
	return -ENOSYS;
}

int kfstat(struct file_desc *desc, struct stat *buff) {
	return -ENOSYS;
}

int kioctl(struct file_desc *fp, int request, void *data) {
	return -ENOSYS;
}

int ktruncate(struct inode *node, off_t length) {
	return -ENOSYS;
}
