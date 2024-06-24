/**
 * @file
 *
 * @date 06.11.15
 * @author Alexander Kalmuk
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_STATFS_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_STATFS_H_

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

#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_STATFS_H_ */
