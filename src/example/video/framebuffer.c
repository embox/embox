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
		0xD0, 0xA0, 0x60, 0x20
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
	fb_fillrect(info, &rect);

	rect.dx = info->var.xres / 2;
	rect.dy = 0;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[1];
//	fb_fillrect(info, &rect);

	rect.dx = 0;
	rect.dy = info->var.yres / 2;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[2];
//	fb_fillrect(info, &rect);

	rect.dx = info->var.xres / 2;
	rect.dy = info->var.yres / 2;
	rect.width = info->var.xres / 2;
	rect.height = info->var.yres / 2;
	rect.color = colors[3];
//	fb_fillrect(info, &rect);

	area.dx = info->var.xres / 2 - 150;
	area.dy = info->var.yres / 2 - 100;
	area.width = 300;
	area.height = 200;
	area.dx = info->var.xres / 2;
	area.dy = info->var.yres / 2;
	area = area;
//	fb_copyarea(info, &area);

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

static int run(int argc, char **argv) {
	if (argc != 2) {
		return -EINVAL;
	}

	return !strcmp(argv[1], "memset") ? framebuffer_memset()
			 : !strcmp(argv[1], "dev") ? framebuffer_dev()
			 : !strcmp(argv[1], "copyarea") ? framebuffer_copyarea()
			 : -EINVAL;
}
