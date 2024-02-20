/**
 * @file
 *
 * @date Jul 1, 2017
 * @author Anton Bondarev
 */

#include <errno.h>
#include <linux/fb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/video/fb.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>

#define DEV_POOL_SIZE OPTION_GET(NUMBER, dev_pool_size)

struct fb_dev {
	struct char_dev cdev;
	struct fb_info *info;
};

POOL_DEF(fb_dev_pool, struct fb_dev, DEV_POOL_SIZE);

static void *fb_direct_access(struct char_dev *cdev, off_t off, size_t len) {
	struct fb_info *info;

	info = ((struct fb_dev *)cdev)->info;

	assert(info);

	return info->screen_base;
}

static int fb_dev_ioctl(struct char_dev *cdev, int request, void *data) {
	struct fb_fix_screeninfo *finfo;
	struct fb_info *info;

	info = ((struct fb_dev *)cdev)->info;

	assert(info);
	assert(data);

	switch (request) {
	case FBIOGET_VSCREENINFO:
		memcpy(data, &info->var, sizeof(struct fb_var_screeninfo));
		break;
	case FBIOGET_FSCREENINFO:
		finfo = data;
		finfo->line_length = info->var.xres * (info->var.bits_per_pixel / 8);
		finfo->smem_start = (uintptr_t)info->screen_base;
		finfo->smem_len = info->screen_size;
		break;
	default:
		return -ENOSYS;
	}

	return ENOERR;
}

static ssize_t fb_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct fb_info *info;

	info = ((struct fb_dev *)cdev)->info;

	assert(info);

	memcpy(buf, info->screen_base, nbyte);

	return nbyte;
}

static ssize_t fb_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	struct fb_info *info;

	info = ((struct fb_dev *)cdev)->info;

	assert(info);

	memcpy(info->screen_base, buf, nbyte);

	return nbyte;
}

static const struct char_dev_ops fb_cdev_ops = {
    .read = fb_read,
    .write = fb_dev_write,
    .ioctl = fb_dev_ioctl,
    .direct_access = fb_direct_access,
};

/* TODO: fb_dev_destroy() */
int fb_devfs_create(struct fb_info *fbi, char *map_base, size_t map_size) {
	struct fb_dev *dev;
	int err;
	char name[NAME_MAX];

	assert(fbi);

	dev = pool_alloc(&fb_dev_pool);
	if (!dev) {
		return -ENOMEM;
	}

	snprintf(name, sizeof(name), "fb%d", fbi->id);

	char_dev_init((struct char_dev *)dev, name, &fb_cdev_ops);

	if ((err = char_dev_register((struct char_dev *)dev))) {
		pool_free(&fb_dev_pool, dev);
	}

	return err;
}
