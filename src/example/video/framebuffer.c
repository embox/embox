/**
 * @file
 * @brief
 *
 * @date 01.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/fb.h>
#include <errno.h>
#include <framework/example/self.h>
#include <stddef.h>
#include <stdio.h>
#include <util/array.h>

EMBOX_EXAMPLE(run);

static const unsigned char colors[] = {
		0xDD, 0xAA, 0x66, 0x32
};

static int framebuffer_turn_on(void) {
	struct fb_info *info;

	info = fb_lookup("fb0");
	if (info == NULL) {
		return -ENODEV;
	}

	assert(info->ops != NULL);
	assert(info->ops->fb_set_par != NULL);
	return info->ops->fb_set_par(info);
}

static int framebuffer_memset(void) {
	int ret;
	size_t i;
	struct fb_info *info;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	info = fb_lookup("fb0");
	if (info == NULL) {
		return -ENODEV;
	}

	for (i = 0; i < ARRAY_SIZE(colors); ++i) {
		fb_memset(info->screen_base + i * info->screen_size / ARRAY_SIZE(colors),
				colors[i], info->screen_size / ARRAY_SIZE(colors));
	}

	return 0;
}

static int framebuffer_copyarea(void) {
	int ret;
	struct fb_fillrect rect;
	struct fb_copyarea area;
	struct fb_info *info;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	info = fb_lookup("fb0");
	if (info == NULL) {
		return -ENODEV;
	}

	rect.dx = 0;
	rect.dy = 0;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[0];
	info->ops->fb_fillrect(info, &rect);

	rect.dx = info->var.xres / 2;
	rect.dy = 0;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[1];
	info->ops->fb_fillrect(info, &rect);

	rect.dx = 0;
	rect.dy = info->var.yres / 2;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[2];
	info->ops->fb_fillrect(info, &rect);

	rect.dx = info->var.xres / 2;
	rect.dy = info->var.yres / 2;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[3];
	info->ops->fb_fillrect(info, &rect);

	area.dx = 20;
	area.dy = 20;
	area.width = 300;
	area.height = 200;
	area.sx = info->var.xres / 2 - 150;
	area.sy = info->var.yres / 2 - 100;
	info->ops->fb_copyarea(info, &area);

	return 0;
}

static int framebuffer_dev(void) {
	int ret;
	size_t i, j, size;
	FILE *fbuf;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	fbuf = fopen("/dev/fb0", "w");
	if (fbuf == NULL) {
		return -errno;
	}

	ret = fseek(fbuf, 0, SEEK_END);
	if (ret != 0) {
		return ret;
	}

	size = ftell(fbuf);
	if ((long int)size == -1L) {
		return -errno;
	}

	ret = fseek(fbuf, 0, SEEK_SET);
	if (ret != 0) {
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(colors); ++i) {
		for (j = 0; j < size / ARRAY_SIZE(colors); ++j) {
			if (fwrite(&colors[i], sizeof colors[0], 1, fbuf) != 1) {
				return -errno;
			}
		}
	}

	fclose(fbuf);

	return 0;
}

static int framebuffer_image(void) {
	int ret;
	size_t size, bytes;
	FILE *fbuf, *fimg;
	char buff[1024];

	fbuf = fimg = NULL;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	fbuf = fopen("/dev/fb0", "w");
	if (fbuf == NULL) goto error;

	ret = fseek(fbuf, 0, SEEK_END);
	if (ret != 0) goto error;

	size = ftell(fbuf);
	if ((long int)size == -1L) goto error;

	ret = fseek(fbuf, 0, SEEK_SET);
	if (ret != 0) goto error;

	fimg = fopen("/image.raw", "r");
	if (fimg == NULL) goto error;

	while (size != 0) {
		bytes = size < sizeof buff ? size : sizeof buff;

		bytes = fread(&buff[0], sizeof buff[0], bytes, fimg);
		if (bytes < 0) goto error;
		else if (bytes == 0) break;

		if (fwrite(&buff[0], sizeof buff[0], bytes, fbuf) != bytes) goto error;

		size -= bytes;
	}

	fclose(fbuf);
	fclose(fimg);

	return 0;
error:
	if (fbuf) fclose(fbuf);
	if (fimg) fclose(fimg);
	return -errno;
}

static int run(int argc, char **argv) {
	if (argc != 2) {
		return -EINVAL;
	}

	return !strcmp(argv[1], "memset") ? framebuffer_memset()
			 : !strcmp(argv[1], "dev") ? framebuffer_dev()
			 : !strcmp(argv[1], "copyarea") ? framebuffer_copyarea()
			 : !strcmp(argv[1], "image") ? framebuffer_image()
			 : -EINVAL;
}
