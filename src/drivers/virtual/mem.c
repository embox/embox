/**
 * @file
 * @brief Creates file /dev/mem
 *
 * @date 29.04.20
 * @author Puranjay Mohan <puranjay12@gmail.com>
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <drivers/char_dev.h>
#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <util/err.h>

#define MEM_DEV_NAME "mem"

static void mem_close(struct idesc *desc) {
}

static ssize_t mem_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	assert(iov);

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		memset(iov[i].iov_base, 0, iov[i].iov_len);

		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t mem_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static void *mem_mmap(struct idesc *idesc, void *addr, size_t len, int prot, int flags, int fd, off_t off){
	void *vmem =  mmap_device_memory(addr, len, prot, flags, off);
	return vmem;
}

static const struct idesc_ops mem_ops = {
	.id_readv  = mem_read,
	.id_writev = mem_write,
	.close     = mem_close,
	.fstat     = char_dev_idesc_fstat,
	.idesc_mmap = mem_mmap,
};

static struct idesc *mem_open(struct dev_module *cdev, void *priv) {
	return char_dev_idesc_create(cdev);
}

CHAR_DEV_DEF(MEM_DEV_NAME, mem_open, NULL, &mem_ops, NULL);
