/**
 * @file
 * @brief Creates file /dev/mem
 *
 * @date 29.04.20
 * @author Puranjay Mohan <puranjay12@gmail.com>
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <drivers/char_dev.h>

static ssize_t mem_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	memset(buf, 0, nbyte);
	return nbyte;
}

static ssize_t mem_write(struct char_dev *cdev, const void *buf, size_t nbyte) {
	return nbyte;
}

static void *mem_direct_access(struct char_dev *cdev, off_t off, size_t len) {
	return (void *)(uintptr_t)off;
}

static const struct char_dev_ops mem_cdev_ops = {
    .read = mem_read,
    .write = mem_write,
    .direct_access = mem_direct_access,
};

static struct char_dev mem_cdev = CHAR_DEV_INIT(mem_cdev, "mem", &mem_cdev_ops);

CHAR_DEV_REGISTER(&mem_cdev);
