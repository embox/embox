/**
 * @file
 *
 * @brief
 *
 * @date 07.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_STAT_H_
#define SYS_STAT_H_

#include <sys/types.h>
#include <fs/node.h>




#define S_IFMT         0170000   /* File type mask */
#define S_IFPKT        0160000   /* Packet device */
#define S_IFSOCK       0140000   /* Socket */
#define S_IFLNK        0120000   /* Symbolic link */
#define S_IFREG        0100000   /* Regular file */
#define S_IFBLK        0060000   /* Block device */
#define S_IFDIR        0040000   /* Directory */
#define S_IFCHR        0020000   /* Character device */
#define S_IFIFO        0010000   /* Pipe */

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK) /* Test for a symbolic link. */
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG) /* Test for a regular file. */
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR) /* Test for a directory. */
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR) /* Test for a character special file. */
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK) /* Test for a block special file. */
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO) /* Test for a pipe or FIFO special file. */
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK) /* Test for a socket. */
#define S_ISPKT(m)      (((m) & S_IFMT) == S_IFPKT)  /* Test for a packet device */

/* not implemented */

#define S_TYPEISMQ(buf)    //Test for a message queue
#define S_TYPEISSEM(buf)   //Test for a semaphore
#define S_TYPEISSHM(buf)   //Test for a shared memory object


#define S_IREAD        0000400   /* Read permission, owner */
#define S_IWRITE       0000200   /* Write permission, owner */
#define S_IEXEC        0000100   /* Execute/search permission, owner */

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

typedef struct stat {
	int       st_dev;     /* ID of device containing file */
	int       st_ino;     /* inode number */
	int       st_mode;    /* protection */
	int       st_nlink;   /* number of hard links */
	int       st_uid;     /* user ID of owner */
	int       st_gid;     /* group ID of owner */
	int       st_rdev;    /* device ID (if special file) */
	size_t    st_size;    /* total size, in bytes */
	size_t    st_blksize; /* blocksize for file system I/O */
	int       st_blocks;  /* number of 512B blocks allocated */
	unsigned  st_atime;   /* time of last access */
	unsigned  st_mtime;   /* time of last modification */
	unsigned  st_ctime;   /* time of last status change */
} stat_t;

typedef struct statfs  {
	unsigned int bsize;        /* Fundamental file system block size */
	unsigned int iosize;       /* Optimal transfer block size */
	unsigned int blocks;       /* Total data blocks in file system */
	unsigned int bfree;        /* Free blocks in fs */
	unsigned int files;        /* Total file nodes in file system */
	unsigned int ffree;        /* Free file nodes in fs */
	unsigned int cachesize;    /* Cache buffers */
	char fstype[MAX_LENGTH_FILE_NAME];   /* File system type name */
	char mntto[MAX_LENGTH_PATH_NAME];       /* Directory on which mounted */
	char mntfrom[MAX_LENGTH_PATH_NAME];     /* Mounted file system */
} statfs_t;

/**
 * Get file status (size, mode, mtime and so on)
 */
static inline int chmod(const char *path, mode_t mode) {
	return -1;
}
extern int    fchmod(int, mode_t);
extern int    stat(const char *, struct stat *);
extern int    lstat(const char *, struct stat *);
extern int    fstat(int fd, struct stat *);
extern int    mkfifo(const char *, mode_t);
extern int    mknod(const char *, mode_t, dev_t);
extern int    mkdir (const char *, mode_t );
static inline mode_t umask(mode_t mode) {
	return 0;
}

#endif /* SYS_STAT_H_ */
