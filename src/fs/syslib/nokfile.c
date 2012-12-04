/**
 * @file
 *
 * @date Nov 27, 2012
 * @author: Anton Bondarev
 */


struct file_desc *kopen(const char *path, int flag) {
	return NULL;
}

size_t kwrite(const void *buf, size_t size, size_t count, struct file_desc *file) {
	return -1;
}

size_t kread(void *buf, size_t size, size_t count, struct file_desc *desc) {
	return -1;
}


int kclose(struct file_desc *desc) {
	return -1;
}

int kseek(struct file_desc *desc, long int offset, int origin) {
	return -1;
}

int kstat(struct file_desc *desc, void *buff) {
	return -1;
}

int kioctl(struct file_desc *fp, int request, ...) {
	return -1;
}
