/**
 * @file
 * @brief POSIX Standard: 2.6 Primitive System Data Types.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_SYS_TYPES_H_
#define COMPAT_POSIX_SYS_TYPES_H_

#ifndef __ASSEMBLER__

#include <stddef.h> /* The <sys/types.h> header shall define size_t */
#include <stdint.h>
#include <sys/cdefs.h>

typedef unsigned long blkcnt_t;   /* file block counts */
typedef unsigned long blksize_t;  /* block sizes */
typedef uint64_t clock_t;         /* jiffies (can overflow if < 64-bit) */
typedef unsigned long fsblkcnt_t; /* file system block counts */
typedef unsigned long fsfilcnt_t; /* file system file counts */
typedef unsigned long ino_t;      /* file serial numbers */
typedef uint32_t clockid_t;       /* clock ID type */
typedef uint32_t timer_t;         /* timer ID */
typedef uint32_t dev_t;           /* device IDs */
typedef uint32_t key_t;           /* XSI interprocess communication */
typedef uint32_t mode_t;          /* some file attributes */
typedef unsigned short gid_t;     /* group IDs */
typedef unsigned short nlink_t;   /* link counts */
typedef unsigned short uid_t;     /* user IDs */
typedef int pid_t;                /* process IDs */
typedef int id_t;                 /*  general identifier */
typedef int ssize_t;              /* a count of bytes or an error */
typedef long off_t;               /* file sizes */
typedef long time_t;              /* time in seconds */
typedef long suseconds_t;         /* time in microseconds (signed) */
typedef unsigned long useconds_t; /* time in microseconds */

/* Not part of posix */
typedef uint64_t loff_t; /* Used for file sizes (64-bit version) */
typedef uint32_t gfp_t;
typedef unsigned int blkno_t;

__BEGIN_DECLS

static inline dev_t makedev(int maj, int min) {
	return ((dev_t)(((maj & 0xFFFF) << 16) | (min & 0xFFFF)));
}

static inline unsigned int major(dev_t dev) {
	return (unsigned int)((dev >> 16) & 0xFFFF);
}

static inline unsigned int minor(dev_t dev) {
	return (unsigned int)(dev & 0xFFFF);
}

__END_DECLS

/* For compatibility with Linux */
#include <bits/types.h>

#endif /* __ASSEMBLER__ */

#endif /* COMPAT_POSIX_SYS_TYPES_H_ */
