/**
 * @file
 * @brief
 *
 * @date 07.09.2011
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_SYS_STAT_H_
#define COMPAT_POSIX_SYS_STAT_H_

#include <limits.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <time.h>

#define S_IFMT           0170000 /* File type mask */
#define S_IFPKT          0160000 /* Packet device */
#define S_IFSOCK         0140000 /* Socket */
#define S_IFLNK          0120000 /* Symbolic link */
#define S_IFREG          0100000 /* Regular file */
#define S_IFBLK          0060000 /* Block device */
#define S_IFDIR          0040000 /* Directory */
#define S_IFCHR          0020000 /* Character device */
#define S_IFIFO          0010000 /* Pipe */

#define __s_is(fmt, m)   (((m)&S_IFMT) == fmt)

#define S_ISPKT(m)       __s_is(S_IFPKT, m)  /* Test for a packet device */
#define S_ISSOCK(m)      __s_is(S_IFSOCK, m) /* Test for a socket. */
#define S_ISLNK(m)       __s_is(S_IFLNK, m)  /* Test for a symbolic link. */
#define S_ISREG(m)       __s_is(S_IFREG, m)  /* Test for a regular file. */
#define S_ISBLK(m)       __s_is(S_IFBLK, m)  /* Test for a block device. */
#define S_ISDIR(m)       __s_is(S_IFDIR, m)  /* Test for a directory. */
#define S_ISCHR(m)       __s_is(S_IFCHR, m)  /* Test for a character device. */
#define S_ISFIFO(m)      __s_is(S_IFIFO, m)  /* Test for a pipe or FIFO. */

/* Permissions: owner, group, other */

#define S_IRWXU          0700 /**< owner: rwx --- --- */
#define S_IRUSR          0400 /**< owner: r-- --- --- */
#define S_IWUSR          0200 /**< owner: -w- --- --- */
#define S_IXUSR          0100 /**< owner: --x --- --- */

#define S_IRWXG          0070 /**< group: --- rwx --- */
#define S_IRGRP          0040 /**< group: --- r-- --- */
#define S_IWGRP          0020 /**< group: --- -w- --- */
#define S_IXGRP          0010 /**< group: --- --x --- */

#define S_IRWXO          0007 /**< other: --- --- rwx */
#define S_IROTH          0004 /**< other: --- --- r-- */
#define S_IWOTH          0002 /**< other: --- --- -w- */
#define S_IXOTH          0001 /**< other: --- --- --x */

/* TODO non-standard, but useful. -- Eldar*/
#define S_IRWXA          0777 /**<   all: rwx rwx rwx */
#define S_IRALL          0444 /**<   all: r-- r-- r-- */
#define S_IWALL          0222 /**<   all: -w- -w- -w- */
#define S_IXALL          0111 /**<   all: --x --x --x */

/* TODO non-standard, BSD compat */
#define S_IREAD          S_IRUSR /* Read permission, owner */
#define S_IWRITE         S_IWUSR /* Write permission, owner */
#define S_IEXEC          S_IXUSR /* Execute/search permission, owner */

/*
 * Special values for utimensat and futimens
 */
#define UTIME_NOW        ((1 << 30) - 1)
#define UTIME_OMIT       ((1 << 30) - 2)

/* TODO this is a embox vfs specific */
#define VFS_DIR_VIRTUAL  0x01000000

/* TODO not implemented */
#define S_TYPEISMQ(buf)  /* Test for a message queue */
#define S_TYPEISSEM(buf) /* Test for a semaphore */
#define S_TYPEISSHM(buf) /* Test for a shared memory object */

#define S_ISUID          0004000 /* set user id on execution */
#define S_ISGID          0002000 /* set group id on execution */
#define S_ISVTX          0001000 /* sticky bit */

#define ALLPERMS \
	(mode_t)(S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)

struct stat {
	dev_t st_dev;         /* ID of device containing file */
	ino_t st_ino;         /* inode number */
	mode_t st_mode;       /* protection */
	nlink_t st_nlink;     /* number of hard links */
	uid_t st_uid;         /* user ID of owner */
	gid_t st_gid;         /* group ID of owner */
	dev_t st_rdev;        /* device ID (if special file) */
	off_t st_size;        /* total size, in bytes */
	blksize_t st_blksize; /* blocksize for file system I/O */
	blkcnt_t st_blocks;   /* number of 512B blocks allocated */

	struct timespec st_atim;    /* time of last access */
	struct timespec st_mtim;    /* time of last modification */
	struct timespec st_ctim;    /* time of last status change */
#define st_atime st_atim.tv_sec /* Backward compatibility */
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
};

/* 
typedef struct statfs {
	unsigned int bsize;     // Fundamental file system block size
	unsigned int iosize;    // Optimal transfer block size
	unsigned int blocks;    // Total data blocks in file system
	unsigned int bfree;     // Free blocks in fs
	unsigned int files;     // Total file nodes in file system
	unsigned int ffree;     // Free file nodes in fs
	unsigned int cachesize; // Cache buffers
	char fstype[NAME_MAX];  // File system type name
	char mntto[PATH_MAX];   // Directory on which mounted
	char mntfrom[PATH_MAX]; // Mounted file system
} statfs_t;
 */

__BEGIN_DECLS

/**
 * Get file status (size, mode, mtime and so on)
 */
extern int chmod(const char *path, mode_t mode);
extern int fchmod(int fildes, mode_t mode);
extern int stat(const char *, struct stat *);
extern int lstat(const char *, struct stat *);
extern int fstat(int fd, struct stat *);
extern int mkfifo(const char *, mode_t);
extern int mknod(const char *, mode_t, dev_t);
extern int mkdir(const char *, mode_t);
extern mode_t umask(mode_t mode);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_STAT_H_ */
