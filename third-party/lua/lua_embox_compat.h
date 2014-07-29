/**
 * @file
 * @brief
 *
 * @date 29.07.14
 * @author Ilia Vaprol
 */

#ifndef LUA_EMBOX_COMPAT_
#define LUA_EMBOX_COMPAT_

//#define LUA_COMPAT_ALL
//#define LUA_USE_POSIX
#define LUA_USE_STRTODHEX

/* Stubs for liolib.c */
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
static inline FILE * tmpfile(void) {
	return SET_ERRNO(ENOSYS), NULL;
}
static inline int setvbuf(FILE *stream, char *buf, int type, size_t size) {
	return SET_ERRNO(ENOSYS);
}

/* Stubs for llex.c */
#define getlocaledecpoint() '.'

/* Stubs for lmathlib.c */
#include <limits.h>
#define RAND_MAX INT_MAX
#define HUGE_VAL (__builtin_huge_val())
static inline double tan(double x) {
	return 0;
}
static inline double tanh(double x) {
	return 0;
}
static inline double ldexp(double x, int exp) {
	return 0;
}

/* Stubs for loslib.c */
#include <time.h>
static inline double difftime(time_t time1, time_t time0) {
	return (double)(time1 - time0);
}

/* Stubs for lvm.c */
#include <string.h>
#define strcoll(s1, s2) strcmp(s1, s2)

#endif /* LUA_EMBOX_COMPAT_ */
