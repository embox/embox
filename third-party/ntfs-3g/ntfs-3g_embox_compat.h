/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef NTFS_EMBOX_COMPAT_H_
#define NTFS_EMBOX_COMPAT_H_


#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#define S_ISVTX 01000

#include <stdio.h>


#define __timespec_defined


// errno values
#define EOVERFLOW 1
#define EMLINK    2
#define ENODATA   3
#define EILSEQ    4
#define E2BIG     5


#ifndef __need_getopt
/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument		(or 0) if the option does not take an argument,
   required_argument	(or 1) if the option requires an argument,
   optional_argument 	(or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument		0
# define required_argument	1
# define optional_argument	2
#endif	/* need getopt */

int getopt_long(int argc, char * const argv[],
           const char *optstring,
           const struct option *longopts, int *longindex);


#include <unistd.h>


#if __WORDSIZE == 32          /* System word size */
# define __SWORD_TYPE           int
#elif  __WORDSIZE == 64
# define __SWORD_TYPE         long int
#else
# error No acceptable __WORDSIZE is defined
#endif


typedef struct { int val[2]; } fsid_t;

struct statfs {
    __SWORD_TYPE f_type;    /* type of file system (see below) */
    __SWORD_TYPE f_bsize;   /* optimal transfer block size */
    fsblkcnt_t   f_blocks;  /* total data blocks in file system */
    fsblkcnt_t   f_bfree;   /* free blocks in fs */
    fsblkcnt_t   f_bavail;  /* free blocks available to
                               unprivileged user */
    fsfilcnt_t   f_files;   /* total file nodes in file system */
    fsfilcnt_t   f_ffree;   /* free file nodes in fs */
    fsid_t       f_fsid;    /* file system id */
    __SWORD_TYPE f_namelen; /* maximum length of filenames */
    __SWORD_TYPE f_frsize;  /* fragment size (since Linux 2.6) */
    __SWORD_TYPE f_spare[5];
};

typedef int sig_atomic_t;

#define UTIME_NOW       ((1l << 30) - 1l)
#define UTIME_OMIT      ((1l << 30) - 2l)


static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
	printf(">>> pread, fd - %d, offset - %d\n", fd, offset);
	return -1;
}

static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	printf(">>> pwrite, fd - %d, offset - %d\n", fd, offset);
	return -1;
}

static inline int major(dev_t dev) {
	printf(">>> major, dev - %d\n", dev);
	return -1;
}

static inline int minor(dev_t dev) {
	printf(">>> minor, dev - %d\n", dev);
	return -1;
}

static inline struct group *getgrgid(gid_t gid) {
	printf(">>> getgrgid %d\n", gid);
	return NULL;
}

//typedef int wchar_t;
#include <wchar.h>
static inline size_t mbstowcs(wchar_t *dest, const char *src, size_t n) {
	printf(">>> %s\n", __func__);
	return 0;
}

//XXX redefine malloc through sysmalloc. Revert it!
#include <stdlib.h>
#define malloc(x)     sysmalloc(x)
#define free(x)       sysfree(x)
#define memalign(x,y) sysmemalign(x,y)
#define realloc(x,y)  sysrealloc(x,y)
#define calloc(x,y)   syscalloc(x,y)

#endif /* NTFS_EMBOX_COMPAT_H_ */
