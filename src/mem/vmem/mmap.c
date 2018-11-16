/**
 * @file mmap.c
 * @brief Various memory mapping
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.02.2018
 */

#include <errno.h>
#include <stddef.h>
#include <sys/mman.h>

#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <kernel/printk.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <mem/sysmalloc.h>
#include <module/embox/fs/syslib/idesc_mmap_api.h>
#include <util/log.h>

extern void *mmap_userspace_add(void *addr, size_t len, int prot);

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	log_debug("addr=%p, len=%d, prot=%d, flags=%d, fd=%d, off=%d", addr, len, prot, flags, fd, off);
	if (len == 0) {
		SET_ERRNO(EINVAL);
		return MAP_FAILED;
	}

	if (flags & MAP_ANONYMOUS) {
		void *ptr = mmap_userspace_add(addr, len, prot);

		if (ptr == NULL) {
			errno = EPERM;
			return MAP_FAILED;
		}

		return ptr;
	} else {
		if (fd > 0) {
			return idesc_mmap(addr, len, prot, flags, fd, off);
		} else {
			return sysmalloc(len);
		}
	}
}
