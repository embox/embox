/**
 * @file
 * @brief Creates diag file in /dev/
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/file.h>
#include <fs/node.h>
#include <kernel/diag.h>
#include <embox/device.h>
#include <diag/diag_device.h>

static void *diag_open(struct file_desc *desc);
static int diag_close(struct file_desc *desc);
static size_t diag_read(void *buf, size_t size, size_t count, void *file);
static size_t diag_write(const void *buff, size_t size, size_t count, void *file);


static file_operations_t file_op = {
		.fread = diag_read,
		.fopen = diag_open,
		.fclose = diag_close,
		.fwrite = diag_write
};

static fs_drv_t fs_drv = {
		.file_op = &file_op
};


static node_t diag_node = {
		.fs_type = &fs_drv
};

FILE *diag_device_get(void) {
	struct file_desc *desc;
	desc = file_desc_alloc();
	desc->ops = &file_op;
	desc->node = &diag_node;

	return (FILE *) desc;
}

/*
 * file_operation
 */
static void *diag_open(struct file_desc *desc) {
	return (void *)&file_op;
}

static int diag_close(struct file_desc *desc) {
	return 0;
}

static size_t diag_read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;

	int i = count * size;

	while (i --) {
		*(ch_buf++) = diag_getc();
	}

	return count * size;
}

static size_t diag_write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;

	while (cnt != count * size) {
		diag_putc(b[cnt++]);
	}
	return cnt;
}

/* doesn't matter if we have fs:
if have, dependency added automaticly;
else this have no effect*/
EMBOX_DEVICE("diag", &file_op);
