/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.02.24
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/poll.h>
#include <sys/stat.h>

#include <framework/mod/options.h>
#include <kernel/task/resource/idesc.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <vfs/core.h>
#include <vfs/dir_idesc.h>

#define IDESC_POOL_SIZE OPTION_GET(NUMBER, idesc_pool_size)

POOL_DEF(dir_idesc_pool, struct dir_idesc, IDESC_POOL_SIZE);

static const struct idesc_ops dir_idesc_ops;

struct dir_idesc *idesc2dir(struct idesc *idesc) {
	assert(idesc);
	assert(idesc->idesc_ops == &dir_idesc_ops);

	return (struct dir_idesc *)idesc;
}

static void dir_idesc_close(struct idesc *idesc) {
	struct dir_idesc *dir;

	if (idesc->idesc_flags & O_PATH) {
		goto out;
	}

	dir = idesc2dir(idesc);

	if (dir->ops->close) {
		dir->ops->close(&dir->inode);
	}

out:
	pool_free(&dir_idesc_pool, idesc);
}

static int dir_idesc_fstat(struct idesc *idesc, struct stat *stat) {
	memset(stat, 0, sizeof(struct stat));
	stat->st_mode = S_IFDIR;

	return 0;
}

static int dir_idesc_status(struct idesc *idesc, int mask) {
	return (int)(mask & (POLLIN | POLLOUT));
}

static int dir_idesc_ioctl(struct idesc *idesc, int request, void *data) {
	return -EISDIR;
}

static ssize_t dir_idesc_readv(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	return -EISDIR;
}

static ssize_t dir_idesc_writev(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	return -EISDIR;
}

static void *dir_idesc_mmap(struct idesc *idesc, void *addr, size_t len,
    int prot, int flags, int fd, off_t off) {
	return err2ptr(EISDIR);
}

static const struct idesc_ops dir_idesc_ops = {
    .close = dir_idesc_close,
    .fstat = dir_idesc_fstat,
    .ioctl = dir_idesc_ioctl,
    .status = dir_idesc_status,
    .id_readv = dir_idesc_readv,
    .id_writev = dir_idesc_writev,
    .idesc_mmap = dir_idesc_mmap,
};

struct idesc *dir_idesc_open(const struct inode *inode,
    const struct dir_idesc_ops *ops, int oflag) {
	struct dir_idesc *dir;

	if ((oflag & O_ACCESS_MASK) != O_RDONLY) {
		return err2ptr(EISDIR);
	}

	dir = pool_alloc(&dir_idesc_pool);
	if (!dir) {
		return err2ptr(ENOMEM);
	}

	idesc_init((struct idesc *)dir, &dir_idesc_ops, oflag);

	if (oflag & O_PATH) {
		dir->inode.ino = VFS_BAD_INO;
		dir->inode.sb = NULL;
		dir->ops = NULL;
	}
	else {
		assert(ops);
		assert(inode);

		dir->ops = ops;
		memcpy(&dir->inode, inode, sizeof(struct inode));
	}

	return (struct idesc *)dir;
}
