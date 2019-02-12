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

#define RTLD_GLOBAL	0x1
#define RTLD_NOW	0x2
#define RTLD_LAZY	0x4
#define RTLD_LOCAL	0x8

extern void  *dlopen(const char *, int);
extern void  *dlsym(void *, const char *);
extern int    dlclose(void *);
extern char  *dlerror(void);

#endif /* DLFCN_H_ */
