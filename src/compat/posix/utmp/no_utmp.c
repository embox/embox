/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <utmp.h>
#include <stddef.h>
#include <errno.h>

struct utmp *getutent(void) {
	errno = ENOSYS;
	return NULL;
}

struct utmp *getutid(struct utmp *ut) {
	errno = ENOSYS;
	return NULL;
}

struct utmp *getutline(struct utmp *ut) {
	errno = ENOSYS;
	return NULL;
}

struct utmp *pututline(struct utmp *ut) {
	errno = ENOSYS;
	return NULL;
}

void setutent(void) {

}

void endutent(void) {

}

int utmpname(const char *file) {
	errno = ENOSYS;
	return -1;
}

