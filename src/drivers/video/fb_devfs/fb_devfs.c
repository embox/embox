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
#include <drivers/device.h>
#include <drivers/video/fb.h>
#include <mem/misc/pool.h>

static void *fb_idesc_mmap(struct idesc *desc, void *addr, size_t len, int prot,
    int flags, int fd, off_t off) {
	struct dev_module *devmod;
	struct fb_info *info;

	assert(desc);

	devmod = idesc_to_dev_module(desc);
	info = devmod->dev_priv;

	assert(info);

	return info->screen_base;
}

static int fb_device_ioctl(struct idesc *desc, int request, void *data) {
	struct dev_module *devmod;
	struct fb_info *info;
	struct fb_fix_screeninfo *finfo;

	assert(desc);
	assert(data);

	devmod = idesc_to_dev_module(desc);
	info = devmod->dev_priv;

	assert(info);

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

static ssize_t fb_id_readv(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	assert(idesc);
	assert(iov);

	struct dev_module *devmod;
	struct fb_info *info;
	int i;
	uint8_t *ptr;

	devmod = idesc_to_dev_module(idesc);
	info = devmod->dev_priv;

	assert(info);

	ptr = (uint8_t *)info->screen_base;

	for (i = 0; i < cnt; i++) {
		memcpy(iov[i].iov_base, ptr, iov[i].iov_len);
		ptr += iov[i].iov_len;
	}

	return (size_t)(ptr - (uint8_t *)info->screen_base);
}

static ssize_t fb_id_writev(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	assert(idesc);
	assert(iov);

	struct dev_module *devmod;
	struct fb_info *info;
	int i;
	uint8_t *ptr;

	devmod = idesc_to_dev_module(idesc);
	info = devmod->dev_priv;

	assert(info);

	ptr = (uint8_t *)info->screen_base;

	for (i = 0; i < cnt; i++) {
		memcpy(ptr, iov[i].iov_base, iov[i].iov_len);
		ptr += iov[i].iov_len;
	}

	return (size_t)(ptr - (uint8_t *)info->screen_base);
}

static const struct dev_module_ops fb_dev_ops;
static const struct idesc_ops fb_idesc_ops;

int fb_devfs_create(struct fb_info *fbi, char *map_base, size_t map_size) {
	struct dev_module *cdev;
	char name[16];

	snprintf(name, sizeof(name), "fb%d", fbi->id);

	assert(fbi);

	cdev = dev_module_create(name, &fb_dev_ops, &fb_idesc_ops, fbi);

	char_dev_register(cdev);

	return 0;
}

static const struct dev_module_ops fb_dev_ops = {
    .dev_open = char_dev_default_open,
};

static const struct idesc_ops fb_idesc_ops = {
    .ioctl = fb_device_ioctl,
    .idesc_mmap = fb_idesc_mmap,
    .id_readv = fb_id_readv,
    .id_writev = fb_id_writev,
    .close = char_dev_default_close,
};
