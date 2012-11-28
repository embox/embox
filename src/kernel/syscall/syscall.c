/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <types.h>
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

