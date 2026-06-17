/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.02.24
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <lib/libds/array.h>
#include <mem/misc/pool.h>
#include <mem/vmem_device_memory.h>
#include <util/err.h>

#define IDESC_POOL_SIZE OPTION_GET(NUMBER, idesc_pool_size)

POOL_DEF(idesc_pool, struct char_dev_idesc, IDESC_POOL_SIZE);

static const struct idesc_ops char_dev_idesc_ops;

static struct char_dev *idesc2cdev(struct idesc *idesc) {
	struct char_dev *cdev;

	assert(idesc);
	assert(idesc->idesc_ops == &char_dev_idesc_ops);

	cdev = ((struct char_dev_idesc *)idesc)->cdev;
	assert(cdev);

	return cdev;
}

static ssize_t char_dev_readv(struct idesc *idesc, const struct iovec *iov,
    int iovcnt) {
	struct char_dev *cdev;
	ssize_t nbyte;
	ssize_t res;
	int i;

	cdev = idesc2cdev(idesc);

	if (!cdev->ops->read) {
		return -EBADF;
	}

	if (idesc->idesc_flags & O_PATH) {
		return -EBADF;
	}

	for (i = 0, nbyte = 0; i < iovcnt; i++) {
		res = cdev->ops->read(cdev, iov[i].iov_base, iov[i].iov_len,
		    idesc->idesc_flags);
		if (res < 0) {
			nbyte = res;
			break;
		}
		nbyte += res;
	}

	return nbyte;
}

static ssize_t char_dev_writev(struct idesc *idesc, const struct iovec *iov,
    int iovcnt) {
	struct char_dev *cdev;
	ssize_t nbyte;
	ssize_t res;
	int i;

	cdev = idesc2cdev(idesc);

	if (!cdev->ops->write) {
		return -EBADF;
	}

	if (idesc->idesc_flags & O_PATH) {
		return -EBADF;
	}

	for (i = 0, nbyte = 0; i < iovcnt; i++) {
		res = cdev->ops->write(cdev, iov[i].iov_base, iov[i].iov_len,
		    idesc->idesc_flags);
		if (res < 0) {
			nbyte = res;
			break;
		}
		nbyte += res;
	}

	return nbyte;
}

void char_dev_close(struct idesc *idesc) {
	struct char_dev *cdev;

	cdev = idesc2cdev(idesc);

	if (idesc->idesc_flags & O_PATH) {
		goto out;
	}

	if (--cdev->usage_count > 0) {
		goto out;
	}

	assert(cdev->usage_count == 0);

	if (cdev->ops->close) {
		cdev->ops->close(cdev);
	}

out:
	pool_free(&idesc_pool, idesc);
}

static int char_dev_ioctl(struct idesc *idesc, int request, void *data) {
	struct char_dev *cdev;

	cdev = idesc2cdev(idesc);

	if (!cdev->ops->ioctl) {
		return -EBADF;
	}

	return cdev->ops->ioctl(cdev, request, data);
}

static int char_dev_fstat(struct idesc *idesc, struct stat *stat) {
	memset(stat, 0, sizeof(struct stat));
	stat->st_mode = S_IFCHR;

	return 0;
}

static int char_dev_status(struct idesc *idesc, int mask) {
	struct char_dev *cdev;

	cdev = idesc2cdev(idesc);

	if (!cdev->ops->status) {
		return -EBADF;
	}

	return cdev->ops->status(cdev, mask);
}

static void *char_dev_mmap(struct idesc *idesc, void *addr, size_t len,
    int prot, int flags, int fd, off_t off) {
	struct char_dev *cdev;
	void *phy_addr;

	cdev = idesc2cdev(idesc);

	if (!cdev->ops->direct_access) {
		return NULL;
	}

	phy_addr = cdev->ops->direct_access(cdev, off, len);
	if (!phy_addr) {
		return NULL;
	}

	mmap_device_memory(phy_addr, len, prot, flags, (uintptr_t)phy_addr);

	return phy_addr;
}

static const struct idesc_ops char_dev_idesc_ops = {
    .id_readv = char_dev_readv,
    .id_writev = char_dev_writev,
    .close = char_dev_close,
    .ioctl = char_dev_ioctl,
    .fstat = char_dev_fstat,
    .status = char_dev_status,
    .idesc_mmap = char_dev_mmap,
};

struct idesc *char_dev_open_idesc(struct char_dev *cdev, int oflag) {
	struct char_dev_idesc *cdev_idesc;
	int err;

	assert(cdev);
	assert(cdev->usage_count >= 0);

	cdev_idesc = pool_alloc(&idesc_pool);
	if (!cdev_idesc) {
		return err2ptr(ENOMEM);
	}

	idesc_init((struct idesc *)cdev_idesc, &char_dev_idesc_ops, oflag);
	cdev_idesc->cdev = cdev;

	if (oflag & O_PATH) {
		goto out;
	}

	if (!cdev->ops->read && ((oflag & O_ACCESS_MASK) != O_WRONLY)) {
		return err2ptr(EACCES);
	}

	if (!cdev->ops->write && ((oflag & O_ACCESS_MASK) != O_RDONLY)) {
		return err2ptr(EACCES);
	}

	if (cdev->usage_count++ > 0) {
		goto out;
	}

	if (cdev->ops->open && (err = cdev->ops->open(cdev, &cdev_idesc->idesc))) {
		pool_free(&idesc_pool, cdev_idesc);
		return err2ptr(-err);
	}

out:
	return (struct idesc *)cdev_idesc;
}

int char_dev_open(const char *name, int oflag) {
	struct char_dev *cdev;
	struct idesc_table *it;
	struct idesc *idesc;

	cdev = char_dev_find(name);
	if (!cdev) {
		return -ENOENT;
	}

	idesc = char_dev_open_idesc(cdev, oflag);
	if (ptr2err(idesc)) {
		return ptr2err(idesc);
	}

	it = task_resource_idesc_table(task_self());

	return idesc_table_add(it, idesc, 0);
}
