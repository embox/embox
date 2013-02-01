/**
 * @file
 * @brief Creates file /dev/fbX
 *
 * @date 29.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/fb.h>
#include <errno.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/node.h>
#include <stddef.h>
#include <string.h>

static int fb_common_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct fb_info *info;

	info = fb_lookup(&node->name[0]);
	assert(info != NULL);

	file_desc->file_info = (void *)info;

	return 0;
}

static int fb_common_close(struct file_desc *desc) {
	return 0;
}

static size_t fb_common_read(struct file_desc *desc, void *buf, size_t size) {
	struct fb_info *info;
	const void *data;
	size_t left;

	info = (struct fb_info *)desc->file_info;
	assert(info != NULL);

	assert(info->ops != NULL);
	if (info->ops->fb_read != NULL) {
		return info->ops->fb_read(info, buf, size, &desc->cursor);
	}

	data = info->screen_base + desc->cursor;
	left = info->screen_size - desc->cursor;
	size = size < left ? size : left;

	fb_memcpy_fromfb(buf, data, size);
	desc->cursor += size;

	return size;
}

static size_t fb_common_write(struct file_desc *desc, void *buf, size_t size) {
	struct fb_info *info;
	void *data;
	size_t left;

	info = (struct fb_info *)desc->file_info;
	assert(info != NULL);

	assert(info->ops != NULL);
	if (info->ops->fb_write != NULL) {
		return info->ops->fb_write(info, buf, size, &desc->cursor);
	}

	data = info->screen_base + desc->cursor;
	left = info->screen_size - desc->cursor;

	if (size > left) {
		return -EFBIG;
	}

	fb_memcpy_tofb(data, buf, size);
	desc->cursor += size;

	return size;
}

static int fb_common_ioctl(struct file_desc *desc, int request, va_list args) {
	int ret;
	struct fb_info *info;

	info = (struct fb_info *)desc->file_info;
	assert(info != NULL);

	switch (request) {
	default:
		assert(info->ops != NULL);
		if (info->ops->fb_ioctl == NULL) {
			return -ENOTTY;
		}
		ret = info->ops->fb_ioctl(info, request, args);
		if (ret != 0) {
			return ret;
		}
		break;
	}

	return 0;
}

const struct kfile_operations fb_common_ops = {
	.open = fb_common_open,
	.close = fb_common_close,
	.read = fb_common_read,
	.write = fb_common_write,
	.ioctl = fb_common_ioctl
};
