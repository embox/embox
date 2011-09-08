/**
 * @file
 * @brief Creates zero file in /dev
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/file.h>
#include <fs/node.h>
#include <kernel/diag.h>
#include <embox/device.h>
#include <diag/diag_device.h>

static void *zero_open(const char *fname, const char *mode);
static int zero_close(void *file);
static size_t zero_read(void *buf, size_t size, size_t count, void *file);


static file_operations_t file_op = {
		.fread = zero_read,
		.fopen = zero_open,
		.fclose = zero_close,
		.fwrite = NULL
};

/*
 * file_operation
 */
static void *zero_open(const char *fname, const char *mode) {
	return (void *)&file_op;
}

static int zero_close(void *file) {
	return 0;
}

static size_t zero_read(void *buf, size_t size, size_t count, void *file) {
	memset(buf, 0, count * size);
	return count * size;
}

EMBOX_DEVICE("zero", &file_op); /* doesn't matter if we have fs */
