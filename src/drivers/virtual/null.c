/**
 * @file
 * @brief Creates null file in /dev
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

static void *null_open(const char *fname, const char *mode);
static int null_close(void *file);
static size_t null_write(const void *buff, size_t size, size_t count, void *file);

static file_operations_t file_op = {
		.fread = NULL,
		.fopen = null_open,
		.fclose = null_close,
		.fwrite = null_write
};

/*
 * file_operation
 */
static void *null_open(const char *fname, const char *mode) {
	return (void *)&file_op;
}

static int null_close(void *file) {
	return 0;
}

static size_t null_write(const void *buff, size_t size, size_t count, void *file) {
	return count * size;
}

EMBOX_DEVICE("zero", &file_op); /* doesn't matter if we have fs */
