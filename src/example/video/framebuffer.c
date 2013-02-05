/**
 * @file
 * @brief
 *
 * @date 01.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <errno.h>
#include <framework/example/self.h>
#include <stddef.h>
#include <stdio.h>
#include <util/array.h>

EMBOX_EXAMPLE(run);

static const unsigned char colors[] = {
		0x07, 0x15, 0x1C, 0x0E
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
	area = area;
//	info->ops->fb_copyarea(info, &area);

	return 0;
}

static int framebuffer_imageblit(void) {
	int ret;
	struct fb_image img;
	struct fb_info *info;
	const char *hello = "Embox> ...", *tmp;
	const struct font_desc *font;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	info = fb_lookup("fb0");
	if (info == NULL) {
		return -ENODEV;
	}

	font = &font_vga_8x8;

	img.dx = 20;
	img.dy = 20;
	img.width = font->width;
	img.height = font->height;
	img.fg_color = 0xF0F0;
	img.bg_color = 0x0;

	img.depth = 1;
	for (tmp = hello; *tmp; ++tmp, img.dx += img.width) {
		img.data = font->data + (unsigned char)(*tmp) * font->width / 8 * font->height;
		info->ops->fb_imageblit(info, &img);
	}

	return 0;
}

static int framebuffer_dev(void) {
	int ret;
	size_t i, j, size;
	FILE *fbuf;
	unsigned short color = 0x26AD;

	ret = framebuffer_turn_on();
	if (ret != 0) {
		return ret;
	}

	fbuf = fopen("/dev/fb0", "w");
	if (fbuf == NULL) {
		fclose(fbuf);
		return -errno;
	}

	ret = fseek(fbuf, 0, SEEK_END);
	if (ret != 0) {
		fclose(fbuf);
		return ret;
	}

	size = ftell(fbuf);
	if ((long int)size == -1L) {
		fclose(fbuf);
		return -errno;
	}

	ret = fseek(fbuf, 0, SEEK_SET);
	if (ret != 0) {
		fclose(fbuf);
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(colors); ++i) {
		for (j = 0; j < size / ARRAY_SIZE(colors); ++j) {
			if (fwrite(&color, sizeof color, 1, fbuf) != 2) {
				fclose(fbuf);
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
			 : !strcmp(argv[1], "imageblit") ? framebuffer_imageblit()
			 : -EINVAL;
}
