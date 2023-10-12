#include <unistd.h>
#include <langinfo.h>
#include <sys/types.h>
#include <sys/statvfs.h>

char *nl_langinfo (nl_item _item) {
	return "UTF-8";
}

int symlink (const char *target, const char *linkpath) {
	return -1;
}

pid_t fork (void) {
	return -1;
}

int fsetflags (const char *p, unsigned long m) {
	return -1;
}

int fgetflags (const char *p, unsigned long *m) {
	return -1;
}

int mkfifo(const char *pathname, mode_t mode) {
	return 0;
}

pid_t getsid(pid_t pid) {
	return -1;
}

pid_t setsid() {
	return -1;
}

int statvfs(const char *path, struct statvfs *buf) {
	return -1;
}
