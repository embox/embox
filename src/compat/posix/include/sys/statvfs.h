/**
 * @file
 *
 * @date 06.11.15
 * @author: Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_FS_SYS_STATVFS_H_
#define SRC_COMPAT_POSIX_FS_SYS_STATVFS_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#define ST_RDONLY 0x1 /* Read-only file system. */
#define ST_NOSUID 0x2 /* Setuid/setgid bits ignored by exec. */

#define FSID_VFAT 0x1
#define FSID_EXT2 0x2
#define FSID_EXT3 0x3
#define FSID_EXT4 0x4

__BEGIN_DECLS

struct statvfs {
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

extern int statvfs(const char *, struct statvfs *);
extern int fstatvfs(int, struct statvfs *);

/* NOT STANDARD */
extern unsigned long fsname2fsid(const char *name);
extern char *fsid2fsname(unsigned long f_sid);

__END_DECLS

#endif /* SRC_COMPAT_POSIX_FS_SYS_STATVFS_H_ */
