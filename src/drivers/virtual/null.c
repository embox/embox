/**
 * @file
 * @brief Creates file /dev/null
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */
#include <stdlib.h>

#include <fs/file_desc.h>
#include <drivers/char_dev.h>
#include <fs/file_operation.h>


#define NULL_DEV_NAME "null"

/* forward declaration */
static int null_init(void);
static const struct file_operations null_ops;

CHAR_DEV_DEF(NULL_DEV_NAME, &null_ops, NULL, null_init);

static struct idesc *null_open(struct node *node, struct file_desc *file_desc, int flags) {
	return &file_desc->idesc;
}

static int null_close(struct file_desc *desc) {
	return 0;
}

static size_t null_write(struct file_desc *desc, void *buf, size_t size) {
	return size;
}

static const struct file_operations null_ops = {
		.open = null_open,
		.close = null_close,
		.write = null_write
};

static int null_init(void) {
	return char_dev_register(NULL_DEV_NAME, &null_ops);
}
