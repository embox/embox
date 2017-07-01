/**
 * @file
 *
 * @date Jul 1, 2017
 * author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/node.h>

#include <drivers/video/fb.h>
#include <linux/fb.h>
#include <drivers/char_dev.h>

struct fb_device {
	const struct fb_ops *fb_ops;
	const struct kfile_operations *kfile_ops;
	void *map_base;
	size_t map_size;
};

static struct fb_device fb_device;
static struct idesc * fb_device_open(struct node *node, struct file_desc *file_desc, int flags) {
	file_desc->file_info = (void *)&fb_device;

	return &file_desc->idesc;
}

static int fb_device_close(struct file_desc *desc) {
	return 0;
}

static size_t fb_device_read(struct file_desc *desc, void *buf, size_t size) {

	return sizeof(char) * size;
}

static size_t fb_device_write(struct file_desc *desc, void *buf, size_t size) {

	return size;
}

static int fb_device_ioctl(struct file_desc *desc, int request, void *data) {
	struct fb_info *info;
	struct fb_device *dev;

	dev = desc->file_info;
	info = fb_lookup(0);

	switch(request) {
	case FBIOGET_VSCREENINFO:
		memcpy(data, &info->var, sizeof(struct fb_var_screeninfo));
	break;
	case FBIOGET_FSCREENINFO: {
		struct fb_fix_screeninfo *finfo = data;

		finfo->line_length = info->var.xres * (info->var.bits_per_pixel / 8);
		finfo->smem_start = (unsigned long)dev->map_base;
		finfo->smem_len = dev->map_size;
	}
	break;
	default:
		return -ENOSYS;
	}
	return ENOERR;
}

static const struct kfile_operations fb_device_ops = {
	.open = fb_device_open,
	.close = fb_device_close,
	.read = fb_device_read,
	.write = fb_device_write,
	.ioctl = fb_device_ioctl
};

int fb_devfs_create(const struct fb_ops *ops, char *map_base, size_t map_size) {
	fb_device.fb_ops = ops;
	fb_device.map_base = map_base;
	fb_device.map_size = map_size;
	fb_device.kfile_ops = &fb_device_ops;
	char_dev_register("fb0", &fb_device_ops);
	return 0;
}
