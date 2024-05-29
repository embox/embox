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
#include <kernel/task/resource/idesc.h>
#include <lib/libds/array.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <util/log.h>

#define IDESC_POOL_SIZE OPTION_GET(NUMBER, idesc_pool_size)

POOL_DEF(idesc_pool, struct char_dev_idesc, IDESC_POOL_SIZE);

static const struct idesc_ops char_dev_idesc_ops;

static bool char_dev_idesc_is_valid(struct idesc *idesc) {
	return idesc && (idesc->idesc_ops == &char_dev_idesc_ops);
}

static struct char_dev *char_dev_idesc_get_dev(struct idesc *idesc) {
	if (!char_dev_idesc_is_valid(idesc)) {
		return NULL;
	}

	return ((struct char_dev_idesc *)idesc)->cdev;
}

static ssize_t char_dev_readv(struct idesc *idesc, const struct iovec *iov,
    int iovcnt) {
	struct char_dev *cdev;
	ssize_t nbyte;
	ssize_t res;
	int i;

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

	if (idesc->idesc_flags & O_PATH) {
		return -EBADF;
	}
	assert(cdev->ops->read);

	for (i = 0, nbyte = 0; i < iovcnt; i++) {
		res = cdev->ops->read(cdev, iov[i].iov_base, iov[i].iov_len);
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

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

	if (idesc->idesc_flags & O_PATH) {
		return -EBADF;
	}
	assert(cdev->ops->write);

	for (i = 0, nbyte = 0; i < iovcnt; i++) {
		res = cdev->ops->write(cdev, iov[i].iov_base, iov[i].iov_len);
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

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

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

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

	if (!cdev->ops->ioctl) {
		return -EBADF;
	}

	return cdev->ops->ioctl(cdev, request, data);
}

static int char_dev_fstat(struct idesc *idesc, struct stat *stat) {
	assert(char_dev_idesc_is_valid(idesc));
	assert(stat);

	memset(stat, 0, sizeof(struct stat));
	stat->st_mode = S_IFCHR;

	return 0;
}

static int char_dev_status(struct idesc *idesc, int mask) {
	struct char_dev *cdev;

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

	if (!cdev->ops->status) {
		return -EBADF;
	}

	return cdev->ops->status(cdev, mask);
}

static void *char_dev_mmap(struct idesc *idesc, void *addr, size_t len,
    int prot, int flags, int fd, off_t off) {
	struct char_dev *cdev;
	void *phy_addr;

	cdev = char_dev_idesc_get_dev(idesc);
	assert(cdev);

	if (!cdev->ops->direct_access) {
		return NULL;
	}

	phy_addr = cdev->ops->direct_access(cdev, off, len);
	if (!phy_addr) {
		return NULL;
	}

	return mmap_device_memory(addr, len, prot, flags,
	    (uint64_t)(uintptr_t)phy_addr);
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

struct idesc *char_dev_open(struct char_dev *cdev, int oflag) {
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
