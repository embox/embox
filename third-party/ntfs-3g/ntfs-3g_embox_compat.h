/**
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

#define __timespec_defined

#define __SWORD_TYPE long int

typedef struct {
	int val[2];
} fsid_t;

struct statfs {
	long f_type;    /* type of file system (see below) */
	long f_bsize;   /* optimal transfer block size */
	long f_blocks;  /* total data blocks in file system */
	long f_bfree;   /* free blocks in fs */
	long f_bavail;  /* free blocks available to unprivileged user */
	long f_files;   /* total file nodes in file system */
	long f_ffree;   /* free file nodes in fs */
	fsid_t f_fsid;  /* file system id */
	long f_namelen; /* maximum length of filenames */
	long f_frsize;  /* fragment size (since Linux 2.6) */
	long f_spare[5];
};

//XXX redefine malloc through sysmalloc. Revert it!
#include <stdlib.h>

#define malloc(x)      sysmalloc(x)
#define free(x)        sysfree(x)
#define memalign(x, y) sysmemalign(x, y)
#define realloc(x, y)  sysrealloc(x, y)
#define calloc(x, y)   syscalloc(x, y)

#endif /* NTFS_EMBOX_COMPAT_H_ */
