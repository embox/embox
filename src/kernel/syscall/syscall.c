/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <kernel/task.h>
#include <kernel/irq.h>
#include <mem/mmap.h>

long sys_exit(int errcode) {
	ipl_enable();
	task_exit(NULL);
}

size_t sys_write(int fd, const void *buf, size_t nbyte) {
	ipl_enable();
	return write(fd, buf, nbyte);
}


#include <string.h>
int sys_open(const char *path, int flags, mode_t mode) {
	ipl_enable();

	if (strcmp(path, "/lib/libselinux.so.1") == 0) {
		return open("libselinux.so.1", flags, mode);
	}

	if (strcmp(path, "/lib/libc.so.6") == 0) {
		return open("libc.so", flags, mode);
	}

	return open(path, flags, mode);

}

int sys_close(int fd) {
	ipl_enable();
	return close(fd);
}

void *sys_mmap2(void *start, size_t length, int prot, int flags, int fd, uint32_t pgoffset) {
	uint32_t offset = pgoffset * 0x1000;
	struct marea *marea;
	void *addr;

	if (start) {
		marea = mmap_place_marea(task_self()->mmap, (uint32_t) start, (uint32_t) start + length, flags);
	} else {
		marea = mmap_alloc_marea(task_self()->mmap, length, flags);
	}

	if (!marea) {
		return (void *) (-1);
	}

	addr = (void *) marea_get_start(marea);

	if (fd != -1) {
		lseek(fd, offset, SEEK_SET);
		read(fd, addr, length);
	}

	return addr;
}


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

          unsigned long   st_atime;
          unsigned long   st_atime_nsec;
          unsigned long   st_mtime;
          unsigned long   st_mtime_nsec;
          unsigned long   st_ctime;
          unsigned long   st_ctime_nsec;
          long            __unused[3];
 };

int sys_newfstat(int fd, void *buf) {
	struct stat stat;
	struct new_stat new_stat;

	int res = fstat(fd, &stat);
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
	new_stat.st_atime = stat.st_atime;
	new_stat.st_mtime = stat.st_mtime;
	new_stat.st_ctime = stat.st_ctime;

	memcpy(buf, &new_stat, sizeof(struct new_stat));
	return res;
}
