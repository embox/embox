#ifndef FIO_OS_EMBOX_H
#define FIO_OS_EMBOX_H

#define FIO_OS	os_embox

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../file.h"

/*
 * Embox is a POSIX-flavoured RTOS.  The port stays on the generic paths
 * and describes only what the kernel provides: a raw block device is a
 * /dev entry whose size comes from fstat().
 */

#define FIO_HAVE_ODIRECT
#define FIO_USE_GENERIC_INIT_RANDOM_STATE

/*
 * Embox's anonymous mmap() is not usable for a command: on this port it
 * hands out addresses aligned to 4K while the kernel's virtual memory
 * page is 64K, so a mapping fio's own allocators make cannot be
 * described by the page tables (the kernel then reports corrupted page
 * table entries).  fio runs single-process here, so the three places
 * that map anonymous memory to hold fio's own structures -- smalloc's
 * pools, the shared semaphores and the rwlocks -- take that memory from
 * the heap instead.
 */
#define FIO_EMBOX_HEAP_ANON

/*
 * The kernel does not run ELF constructors, so fio's fio_init() hooks --
 * including the ones that register the built-in ioengines -- never fire.
 * The engines this port builds in are registered from initialize_fio().
 */
#define FIO_EMBOX_NO_CTORS

/*
 * <pthread.h> has no PTHREAD_SCOPE_PROCESS/PTHREAD_SCOPE_SYSTEM values:
 * the kernel has a single contention scope and the attribute would be a
 * no-op, so the idle profiling setup skips that call.
 */
#define FIO_EMBOX_PTHREAD_NO_SCOPE

#define OS_MAP_ANON	MAP_ANONYMOUS

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 4096
#endif

/*
 * The block layer's own size ioctl answers with an int, which a medium
 * larger than 2 GiB does not fit, so the driver reports the byte size
 * through a command of its own (see drivers/ahci/dwc_ahci.c).  The block
 * layer and fstat are the fallbacks for a device that offers only those.
 */
#define FIO_EMBOX_IOCTL_GET_SIZE_BYTES 0x41484349
#define FIO_EMBOX_IOCTL_GETDEVSIZE     2

static inline int blockdev_size(struct fio_file *f, unsigned long long *bytes)
{
	unsigned long long size = 0;
	struct stat st;
	long ret;

	if (ioctl(f->fd, FIO_EMBOX_IOCTL_GET_SIZE_BYTES, &size) == 0 && size > 0) {
		*bytes = size;
		return 0;
	}

	ret = ioctl(f->fd, FIO_EMBOX_IOCTL_GETDEVSIZE, NULL);
	if (ret > 0) {
		*bytes = (unsigned long long) ret;
		return 0;
	}

	if (fstat(f->fd, &st) == 0 && st.st_size > 0) {
		*bytes = (unsigned long long) st.st_size;
		return 0;
	}

	*bytes = 0;
	return ENOTSUP;
}

static inline int blockdev_invalidate_cache(struct fio_file *f)
{
	/*
	 * The block device idesc hands requests straight to the driver,
	 * there is no cache to drop.
	 */
	return 0;
}

static inline unsigned long long os_phys_mem(void)
{
	long pagesize;
	long pages;

	pagesize = sysconf(_SC_PAGESIZE);
	pages = sysconf(_SC_PHYS_PAGES);
	if (pagesize <= 0 || pages <= 0) {
		return 0;
	}

	return (unsigned long long) pagesize * (unsigned long long) pages;
}

#endif
