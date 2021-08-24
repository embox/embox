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

#include <stdio.h>


#define __timespec_defined



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


//XXX redefine malloc through sysmalloc. Revert it!
#include <stdlib.h>

#define malloc(x)     sysmalloc(x)
#define free(x)       sysfree(x)
#define memalign(x,y) sysmemalign(x,y)
#define realloc(x,y)  sysrealloc(x,y)
#define calloc(x,y)   syscalloc(x,y)

#endif /* NTFS_EMBOX_COMPAT_H_ */
