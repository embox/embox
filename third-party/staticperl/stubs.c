#include <signal.h>
#include <errno.h>

int sigpending(sigset_t *set) {
	return SET_ERRNO(ENOSYS);
}

int sigsuspend(const sigset_t *mask) {
	return SET_ERRNO(ENOSYS);
}

int pause(void) {
	return SET_ERRNO(ENOSYS);
}

char *ttyname(int fd) {
	errno = ENOSYS;
	return NULL;
}

void tzset(void) {
}
