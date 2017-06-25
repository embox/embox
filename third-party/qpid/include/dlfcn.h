/*
 * dlfcn.h
 *
 *  Created on: 2013-okt-23
 *      Author: fsulima
 */

#ifndef DLFCN_H_
#define DLFCN_H_

#include <errno.h>

#define RTLD_NOW	0x2

static inline
void  *dlopen(const char *, int) {
	DPRINT();
	errno = ENOMEM;
	return NULL;
}
extern void  *dlsym(void *, const char *) {
	DPRINT();
	errno = ENOMEM;
	return NULL;
}
extern int    dlclose(void *) {
	DPRINT();
	errno = ENOMEM;
	return -1;
}
extern char  *dlerror(void) {
	DPRINT();
	return "dl* functions are not implemented";
}


#endif /* DLFCN_H_ */
