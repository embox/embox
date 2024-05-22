/**
 * @file
 *
 * @date 06.11.15
 * @author Alexander Kalmuk
 */

#ifndef SYS_STATFS_H_
#define SYS_STATFS_H_

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

#endif /* SYS_STATFS_H_ */
