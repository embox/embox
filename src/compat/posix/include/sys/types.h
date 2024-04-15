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

#include <defines/clock_t.h> /* Used for system times in clock ticks or CLOCKS_PER_SEC */
#include <defines/clockid_t.h>
#include <defines/size_t.h>
#include <defines/suseconds_t.h> /* Used for time in microseconds */
#include <defines/time_t.h>
#include <defines/timer_t.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include <asm/types.h>

/** Note also that 'size_t' is a typedef required by the ISO C standard
 (it must be available if <stddef.h> is included). However, 'ssize_t'
 does not exist in the C standard -- the standard 'ptrdiff_t'
 typedef is nearly equivalent.*/
typedef int ssize_t;

/** Used for time in microseconds.*/
typedef unsigned long useconds_t;

typedef uint32_t gfp_t;

typedef int id_t;

typedef int pid_t;

typedef uint32_t dev_t;

typedef int key_t;

typedef unsigned long ino_t;

typedef unsigned short nlink_t;

typedef uint32_t mode_t;

typedef unsigned short uid_t;
typedef unsigned short gid_t;

typedef unsigned int blkno_t;

typedef long long off_t; /*  Used for file sizes. */

typedef long long loff_t;

typedef unsigned long fsblkcnt_t;
typedef unsigned long fsfilcnt_t;

typedef unsigned long blkcnt_t;
typedef unsigned long blksize_t;

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

#endif /* __ASSEMBLER__ */

#endif /* COMPAT_POSIX_SYS_TYPES_H_ */
