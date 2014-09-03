
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

//#define RAND_MAX 32767

#define HUGE_VAL 0


// because of printf
#include <stdio.h>


#include <sys/types.h> // for size_t
#include <sys/time.h> // for struct timeval
#include <linux/types.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/mman.h>

// because of strcmp
#include <string.h>
// this function has to be added to string.h
static inline int strcoll(const char *s1, const char *s2) {
	printf(">>> strcoll(%s,%s)\n", s1, s2);
	return strcmp(s1, s2);
}

// The following definitions are needed only for testlib and are not linked

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

#include <sys/cdefs.h>
__BEGIN_DECLS
static inline pid_t fork(void) {
	return -1;
}
__END_DECLS
