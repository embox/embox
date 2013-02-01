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
		0xF0, 0x0F, 0x70, 0x07
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
			 : -EINVAL;
}
