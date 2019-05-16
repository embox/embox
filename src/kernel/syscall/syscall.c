/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <kernel/task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <assert.h>

long sys_exit(int errcode) {
	task_exit(NULL);
}

size_t sys_write(int fd, const void *buf, size_t nbyte) {
	return write(fd, buf, nbyte);
}

int sys_open(const char *path, int flags, mode_t mode) {
	return open(path, flags, mode);
}

int sys_close(int fd) {
	return close(fd);
}

void *sys_brk(void *new_brk) {
	if (!new_brk) {
		return mmap_get_brk(task_self_resource_mmap());
	} else {
		mmap_set_brk(task_self_resource_mmap(), new_brk);
		return new_brk;
	}
}

void *sys_mmap2(void *start, size_t length, int prot, int flags, int fd, uint32_t pgoffset) {
	uint32_t offset = pgoffset * 0x1000;

	if (!start) {
		start = (void *) mmap_alloc(EMMAP_SELF, length);

		if (start == NULL) {
			return (void *) -1;
		}
	}

	if (mmap_place(EMMAP_SELF,
				(uintptr_t) start, length, flags)) {
		return (void *) (-1);
	}

	if (fd != -1) {
		lseek(fd, offset, SEEK_SET);
		read(fd, start, length);
	}

	return start;
}

#if 0
struct new_stat {
          unsigned long   st_dev;
          unsigned long   st_ino;
          unsigned long   st_nlink;

          unsigned int    st_mode;
          unsigned int    st_uid;
          unsigned int    st_gid;
          unsigned int    __pad0;
          unsigned long   st_rdev;
          long            st_size;
          long            st_blksize;
          long            st_blocks;      /* Number 512-byte blocks allocated. */

          unsigned long   n_st_atime;
          unsigned long   st_atime_nsec;
          unsigned long   n_st_mtime;
          unsigned long   st_mtime_nsec;
          unsigned long   n_st_ctime;
          unsigned long   st_ctime_nsec;
          long            __unused[3];
 };
#else
struct new_stat
{
  short		st_dev;
  unsigned short st_ino;
  unsigned int	st_mode;
  unsigned short st_nlink;
  unsigned short st_uid;
  unsigned short st_gid;
  short		st_rdev;
  long		st_size;
  long	n_st_atime;
  long		st_spare1;
  long	n_st_mtime;
  long		st_spare2;
  long	n_st_ctime;
  long		st_spare3;
  long		st_blksize;
  long		st_blocks;
  long	st_spare4[2];
};
#endif

static_assert(sizeof(struct new_stat) == 60);

int sys_newfstat(int fd, void *buf) {
	struct stat stat;
	struct new_stat new_stat;

	int res = fstat(fd, &stat);

	memset(&new_stat, 0, sizeof(struct new_stat));

	new_stat.st_dev = stat.st_dev;
	new_stat.st_ino = stat.st_ino;
	new_stat.st_mode = stat.st_mode;
	new_stat.st_nlink = stat.st_nlink;
	new_stat.st_uid = stat.st_uid;
	new_stat.st_gid = stat.st_gid;
	new_stat.st_rdev = stat.st_rdev;
	new_stat.st_size = stat.st_size;
	new_stat.st_blksize = stat.st_blksize;
	new_stat.st_blocks = stat.st_blocks;
	new_stat.n_st_atime = stat.st_atime;
	new_stat.n_st_mtime = stat.st_mtime;
	new_stat.n_st_ctime = stat.st_ctime;

	memcpy(buf, &new_stat, sizeof(struct new_stat));
	return res;
}
