/**
 * @file
 * @brief Creates file /dev/zero
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <embox/device.h>
#include <fs/file_operation.h>
#include <stdlib.h>
#include <string.h>

#define ZERO_DEV_NAME "zero"

/* forward declaration */
static int zero_init(void);
static const struct kfile_operations zero_ops;

EMBOX_DEVICE(ZERO_DEV_NAME, &zero_ops, zero_init);

static int zero_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int zero_close(struct file_desc *desc) {
	return 0;
}

static size_t zero_read(struct file_desc *desc, void *buf, size_t size) {
	memset(buf, 0, size);
	return size;
}

static size_t zero_write(struct file_desc *desc, void *buf, size_t size) {
	return size;
}

static const struct kfile_operations zero_ops = {
		.open = zero_open,
		.close = zero_close,
		.read = zero_read,
		.write = zero_write
};

static int zero_init(void) {
	return char_dev_register(ZERO_DEV_NAME, &zero_ops);
}
