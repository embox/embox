/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

void  syslog(int prio, const char *format, ...) {
}

void openlog(const char *ident, int option, int facility) {
}


int dn_expand(unsigned char *msg, unsigned char *eomorig,
              unsigned char *comp_dn, char *exp_dn,
              int length) {
	return -1;
}

int res_query(const char *dname, int class, int type,
              unsigned char *answer, int anslen) {
	return -1;
}

char *mktemp(char *template) {
	if (template) {
		*template = 0;
	}
	return template;
}

unsigned int alarm(unsigned int seconds) {
	return 0;
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
	errno = ENOSYS;
	return -1;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
	int i;
	size_t bw = 0;
	for(i=0; i<iovcnt; i++) {
		int res;
		if (iov[i].iov_len) {
			res = write(fd, iov[i].iov_base, iov[i].iov_len);
		} else {
			res = 0;
		}
		if (res<0) {
			return -1;
		}
		if (res != iov[i].iov_len) {
			errno = EIO;
			return -1;
		}
		bw += res;
	}
	return bw;
}


#include <dirent.h>
void seekdir(DIR *dirp, long offset) {
}

long telldir(DIR *dirp) {
	errno = EPERM;
	return -1;
}


void setgrent(void) {
}

struct group *getgrent(void) {
	errno = EPERM;
	return 0;
}

void endgrent(void) {
}

int fnmatch(const char *pattern, const char *string, int flags) {
	return -1;
}

void atexit(void *addr) {
}


int fork(void) {
	return -1;
}
