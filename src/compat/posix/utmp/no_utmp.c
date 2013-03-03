/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <utmp.h>
#include <stddef.h>

struct utmp *getutent(void) {
	return NULL;
}

struct utmp *getutid(struct utmp *ut) {
	return NULL;
}

struct utmp *getutline(struct utmp *ut) {
	return NULL;
}

struct utmp *pututline(struct utmp *ut) {
	return NULL;
}

void setutent(void) {

}

void endutent(void) {

}

int utmpname(const char *file) {
	return -1;
}

