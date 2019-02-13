/**
 * @file
 *
 * @date Jul 1, 2017
 * author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/node.h>

#include <fs/idesc.h>

#include <drivers/video/fb.h>
#include <linux/fb.h>
#include <drivers/char_dev.h>

#define FB_POOL_SIZE 1

struct fb_device {
	const struct fb_ops *fb_ops;
	const struct file_operations *kfile_ops;
	void *map_base;
	size_t map_size;
};

static struct fb_device fb_device;

static void fb_device_close(struct idesc *idesc) {
}

static void *
fb_idesc_mmap(struct idesc *idesc, void *addr, size_t len, int prot, int flags,
		int fd, off_t off) {
	return fb_device.map_base;
}

static int fb_device_ioctl(struct idesc *desc, int request, void *data) {
	struct fb_info *info;
	struct fb_device *dev;

	dev = &fb_device;
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


static const struct idesc_ops fb_idesc_ops = {
	.id_readv = NULL,
	.id_writev = NULL,
	.close = fb_device_close,
	.ioctl = fb_device_ioctl,
	.fstat = NULL,
	.status = NULL,
	.idesc_mmap = fb_idesc_mmap
};

static struct idesc * fb_device_open(struct node *node, struct file_desc *file_desc, int flags) {
	file_desc->file_info = (void *)&fb_device;

	file_desc->idesc.idesc_ops = &fb_idesc_ops;
	return &file_desc->idesc;
}

static const struct file_operations fb_device_ops = {
	.open = fb_device_open,
};

POOL_DEF(cdev_fb_pool, struct dev_module, FB_POOL_SIZE);

int fb_devfs_create(const struct fb_ops *ops, char *map_base, size_t map_size) {
	struct dev_module *cdev;

	cdev = pool_alloc(&cdev_fb_pool);
	if (!cdev) {
		return -ENOMEM;
	}
	memset(cdev, 0, sizeof(*cdev));
	memcpy(cdev->name, "fb0", sizeof(cdev->name));
	cdev->dev_iops = &fb_idesc_ops;

	fb_device.fb_ops = ops;
	fb_device.map_base = map_base;
	fb_device.map_size = map_size;
	fb_device.kfile_ops = &fb_device_ops;
	
	char_dev_register(cdev);
	return 0;
}
