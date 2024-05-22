/**
 * @file
 *
 * @date 06.11.15
 * @author Alexander Kalmuk
 */

#ifndef SYS_STATFS_H_
#define SYS_STATFS_H_

#include <sys/types.h>

struct statfs {
	unsigned long f_bsize;   /* File system block size. */
	unsigned long f_frsize;  /* Fundamental file system block size. */
	fsblkcnt_t    f_blocks;  /* Total number of blocks on file system in units of f_frsize. */
	fsblkcnt_t    f_bfree;   /* Total number of free blocks. */
	fsblkcnt_t    f_bavail;  /* Number of free blocks available to non-privileged process. */
	fsfilcnt_t    f_files;   /* Total number of file serial numbers. */
	fsfilcnt_t    f_ffree;   /* Total number of free file serial numbers. */
	fsfilcnt_t    f_favail;  /* Number of file serial numbers available to non-privileged process. */
	unsigned long f_fsid;    /* File system ID. */
	unsigned long f_flag;    /* Bit mask of f_flag values. */
	unsigned long f_namemax; /* Maximum filename length. */
};



#define ST_RDONLY 0x1 /* Read-only file system. */
#define ST_NOSUID 0x2 /* Setuid/setgid bits ignored by exec. */

extern int statfs(const char *, struct statfs *);
extern int fstatfs(int, struct statfs *);

#endif /* SYS_STATFS_H_ */
