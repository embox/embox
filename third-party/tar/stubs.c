#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <kernel/printk.h>
#include <dirent.h>
#include <utime.h>

int link(const char *oldpath, const char *newpath) {
	printk(">>> %s %s %s\n", __func__, oldpath, newpath);
	return SET_ERRNO(ENOSYS);
}

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
	printk(">>> %s %s %s %zu\n", __func__, path, buf, bufsize);
	return SET_ERRNO(ENOSYS);
}

ssize_t readlinkat(int fd, const char *path, char *buf,
		size_t bufsize) {
	printk(">>> %s %d %s %s %zu\n", __func__, fd, path, buf,
			bufsize);
	return SET_ERRNO(ENOSYS);
}

int WIFEXITED(int status) {
	printk(">>> %s %d\n", __func__, status);
	return 0;
}

void flockfile(FILE *file) {
	printk(">>> %s %p\n", __func__, file);
}

int ftrylockfile(FILE *file) {
	printk(">>> %s %p\n", __func__, file);
	return 0;
}

void funlockfile(FILE *file) {
	printk(">>> %s %p\n", __func__, file);
}

void rewinddir(DIR *dirp) {
	printk(">>> %s %p\n", __func__, dirp);
}

int utime(const char *path,
		const struct utimbuf *times) {
	printk(">>> %s %s %p\n", __func__, path, times);
	return SET_ERRNO(ENOSYS);
}

int execl(const char *path, const char *arg0,
		... /*, (char *)0 */) {
	printk(">>> %s %s %s\n", __func__, path, arg0);
	return SET_ERRNO(ENOSYS);
}

int execlp(const char *file, const char *arg0,
		... /*, (char *)0 */) {
	printk(">>> %s %s %s\n", __func__, file, arg0);
	return SET_ERRNO(ENOSYS);
}

int execvp(const char *file, char *const argv[]) {
	printk(">>> %s %s %s\n", __func__, file, argv[0]);
	return SET_ERRNO(ENOSYS);
}

int getdtablesize(void) {
	printk(">>> %s\n", __func__);
	return 10;
}

#if 0
int chdir_long(char *dir) {
	printk(">>> %s %s\n", __func__, dir);
	return 0;
}
#endif

