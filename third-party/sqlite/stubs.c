#include <errno.h>
#include <kernel/printk.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>

int getrusage(int who, struct rusage *r_usage) {
	printk(">>> %s %d %p\n", __func__, who, r_usage);
	memset(r_usage, 0, sizeof *r_usage);
	return SET_ERRNO(ENOSYS);
}

FILE * popen(const char *command, const char *mode) {
	printk(">>> %s %s %s\n", __func__, command, mode);
	return SET_ERRNO(ENOSYS), NULL;
}

int pclose(FILE *stream) {
	printk(">>> %s %p\n", __func__, stream);
	return SET_ERRNO(ENOSYS);
}

int fchown(int fildes, uid_t owner, gid_t group) {
	printk(">>> %s %d %u %u\n", __func__, fildes, owner, group);
	return SET_ERRNO(ENOSYS);
}

int fchmod(int fildes, mode_t mode) {
	printk(">>> %s %d %u\n", __func__, fildes, mode);
	return SET_ERRNO(ENOSYS);
}

int utimes(const char *path, const struct timeval times[2]) {
	printk(">>> %s %s %p\n", __func__, path, times);
	return SET_ERRNO(ENOSYS);
}

void * mmap(void *addr, size_t len, int prot, int flags,
		int fildes, off_t off) {
	printk(">>> %s %p %zu %d %d %d %zd\n", __func__, addr, len,
			prot, flags, fildes, off);
	return SET_ERRNO(ENOSYS), MAP_FAILED;
}

void * mremap(void *old_addr, size_t old_len, size_t new_len,
		int flags, ... /* void *new_addr */) {
	printk(">>> %s %p %zu %zu %d\n", __func__, old_addr, old_len,
			new_len, flags);
	return SET_ERRNO(ENOSYS), MAP_FAILED;
}

int munmap(void *addr, size_t len) {
	printk(">>> %s %p %zu\n", __func__, addr, len);
	return SET_ERRNO(ENOSYS);
}
