/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <linux/init.h>
#include <embox/kernel.h>

static FILE_DESC fdesc[CONFIG_MAX_FILE_QUANTITY];

static void *fopen(const char *path, const char *mode);
static int fclose(void * file);
static size_t fread(void *buf, size_t size, size_t count, void *file);
static size_t fwrite(const void *buf, size_t size, size_t count, void *file);
static int fseek(void *file, long offset, int whence);

static file_op_t fop = {
	fopen,
	fclose,
	fread,
	fwrite,
	fseek,
	ioctrl
};

static int init(void) {
}


static int create(void *params) {
	return 0;
}

static int delete(const char * file_name) {
	return 0;
}

static fsop_desc_t fsop = {
	init,
	create,
	delete
};

static file_system_driver_t drv = {"rootfs", &fop, &fsop};
DECLARE_FILE_SYSTEM_DRIVER(drv);

static void *_fopen(const char *file_name, const char *mode) {
	return NULL;
}

static int fclose(void * file) {
	return 0;
}

static size_t fread(void *buf, size_t size, size_t count, void *file) {
	return 0;
}

static size_t fwrite(const void *buf, size_t size, size_t count, void *file) {
	return 0;
}

static int fseek(void *file, long offset, int whence) {
	return 0;
}
