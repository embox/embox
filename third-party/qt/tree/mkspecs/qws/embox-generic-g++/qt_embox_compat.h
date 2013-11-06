
#ifdef configure
#undef configure
#endif

#if defined(__linux__)
#undef __linux__
#endif

#if defined(__linux)
#undef __linux
#endif

/*
#if defined(__STDC__)
// Causes problems with fdlibm
#undef __STDC__
#endif
*/

//#define __UCLIBC__
//#define QT_LINUXBASE

// #define __MAKEDEPEND__

#define RAND_MAX 32767

#define HUGE_VAL 0


// because of printf
#include <stdio.h>


#include <sys/types.h> // for size_t
#include <sys/time.h> // for struct timeval
#include <linux/types.h>
#include <stddef.h>
#include <stdint.h>

#define MAP_SHARED    0x00
#define MAP_PRIVATE   0x01
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define MAP_FAILED    (-1)
#include <errno.h>
static inline void  *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	// ToDo: implement for InitFS files
	(void)addr;
	(void)len;
	(void)prot;
	(void)flags;
	(void)off;
	//printf(">>> mmap(%i)\n",fd);
	errno = EPERM;
	return NULL;
}
static inline int munmap(void *addr, size_t size) {
	(void)size;
	printf(">>> munmap(%p)\n",addr);
	errno = EPERM;
	return -1;
}
/* implemented now
#include <time.h>
extern int nanosleep(const struct timespec *req, struct timespec *rem);
*/

#include <string.h>
// Stuff below moved here because of testlib

//#define EMBOX_OVERRIDE_GETENV
/*static inline char *getenv(const char *name) {
	printf(">>> getenv(%s)\n",name);
	if (strcmp(name, "QT_QPA_FONTDIR") == 0) {
		return "/";
	}
	return 0;
}*/
/*static inline int fflush(FILE *x) {
	//printf(">>> fflush(%d)\n",(int)x);
	return EOF;
}*/

// because of strcmp
#include <string.h>
// this function has to be added to string.h
static inline int strcoll(const char *s1, const char *s2) {
	printf(">>> strcoll(%s,%s)\n", s1, s2);
	return strcmp(s1, s2);
}

// The following definitions are needed only for testlib and are not linked
#define SA_RESETHAND 0


/*
#define SA_NOCLDSTOP 0
*/


// Required by libtiff
static inline void *
lfind(const void *key, const void *base, size_t *nmemb, size_t size,
      int(*compar)(const void *, const void *))
{
	char *element, *end;

	end = (char *)base + *nmemb * size;
	for (element = (char *)base; element < end; element += size)
		if (!compar(element, key))		/* key found */
			return element;

	return NULL;
}
