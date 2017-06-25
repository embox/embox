/**
 * @file
 *
 * @brief
 *
 * @date 12.09.2011
 * @author Anton Bondarev
 */

#ifndef DLFCN_H_
#define DLFCN_H_

#define RTLD_NOW	0x2

extern void  *dlopen(const char *, int);
extern void  *dlsym(void *, const char *);
extern int    dlclose(void *);
extern char  *dlerror(void);

#endif /* DLFCN_H_ */
