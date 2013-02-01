/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/fb.h>
#include <embox/device.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODOPS_FB_AMOUNT OPTION_GET(NUMBER, fb_amount)

POOL_DEF(fb_pool, struct fb_info, MODOPS_FB_AMOUNT);
static struct fb_info *registered_fb[MODOPS_FB_AMOUNT];
static unsigned int num_registered_fb = 0;

extern const struct kfile_operations fb_common_ops;

struct fb_info * fb_alloc(void) {
	struct fb_info *info;

	info = pool_alloc(&fb_pool);
	if (info == NULL) {
		return NULL;
	}

	return info;
}

void fb_release(struct fb_info *info) {
	assert(info != NULL);
	pool_free(&fb_pool, info);
}

int fb_register(struct fb_info *info) {
	unsigned int num;
	char name[] = "fbXX";

	assert(info != NULL);

	if (num_registered_fb == sizeof registered_fb / sizeof registered_fb) {
		return -ENOMEM;
	}

	++num_registered_fb;

	for (num = 0; (num < num_registered_fb) && (registered_fb[num] != NULL); ++num);

	info->node = num;
	registered_fb[num] = info;

	snprintf(&name[0], sizeof name, "fb%u", num);
	return char_dev_register(&name[0], &fb_common_ops);
}

int fb_unregister(struct fb_info *info) {
	assert(info != NULL);
	assert(registered_fb[info->node] == info);
	registered_fb[info->node] = NULL;
	return 0;
}

struct fb_info * fb_lookup(const char *name) {
	unsigned int num;

	assert(name != NULL);

	sscanf(name, "fb%u", &num);

	assert(num < sizeof registered_fb / sizeof registered_fb[0]);

	return registered_fb[num];
}

int fb_set_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	int ret;
	struct fb_var_screeninfo old;

	assert(info != NULL);
	assert(var != NULL);

	if (info->ops->fb_check_var == NULL) {
		memcpy(var, &info->var, sizeof(struct fb_var_screeninfo));
		return 0;
	}

	ret = info->ops->fb_check_var(var, info);
	if (ret != 0) {
		return ret;
	}

	memcpy(&old, &info->var, sizeof(struct fb_var_screeninfo));
	memcpy(&info->var, var, sizeof(struct fb_var_screeninfo));

	if (info->ops->fb_set_par) {
		ret = info->ops->fb_set_par(info);
		if (ret != 0) {
			memcpy(&info->var, &old, sizeof(struct fb_var_screeninfo));
			return ret;
		}
	}

	return 0;
}

void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area) {
	uint32_t size_x, size_y, j;
	char *base;

	assert(info != NULL);
	assert(area != NULL);

	size_x = area->width;
	size_x = area->sx + size_x > info->var.xres
			? info->var.xres - area->sx : size_x;
	size_x = area->dx + size_x > info->var.xres
			? info->var.xres - area->dx : size_x;

	size_y = area->height;
	size_y = area->sy + size_y > info->var.yres
			? info->var.yres - area->sy : size_y;
	size_y = area->dy + size_y > info->var.yres
			? info->var.yres - area->dy : size_y;

	base = (char *)info->screen_base;
	assert(base != NULL);

	if (area->sx <= area->dx) {
		if (area->sy < area->dy) {
			for (j = size_y - 1; j >= 0; --j) {
				memmove(base + (area->dx + (j + area->dy) * info->var.xres) * info->var.bits_per_pixel / 8,
						base + (area->sx + (j + area->sy) * info->var.xres) * info->var.bits_per_pixel / 8,
						size_x * info->var.bits_per_pixel / 8);
			}
		}
		else {
			for (j = 0; j < size_y; ++j) {
				memmove(base + (area->dx + (j + area->dy) * info->var.xres) * info->var.bits_per_pixel / 8,
						base + (area->sx + (j + area->sy) * info->var.xres) * info->var.bits_per_pixel / 8,
						size_x * info->var.bits_per_pixel / 8);
			}
		}
	}
	else {
		if (area->sy < area->dy) {
			for (j = size_y - 1; j >= 0; --j) {
				memcpy(base + (area->dx + (j + area->dy) * info->var.xres) * info->var.bits_per_pixel / 8,
						base + (area->sx + (j + area->sy) * info->var.xres) * info->var.bits_per_pixel / 8,
						size_x * info->var.bits_per_pixel / 8);
			}
		}
		else {
			for (j = 0; j < size_y; ++j) {
				memcpy(base + (area->dx + (j + area->dy) * info->var.xres) * info->var.bits_per_pixel / 8,
						base + (area->sx + (j + area->sy) * info->var.xres) * info->var.bits_per_pixel / 8,
						size_x * info->var.bits_per_pixel / 8);
			}
		}
	}
}

void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {
	uint32_t size_x, size_y, j;
	char *base;

	assert(info != NULL);
	assert(rect != NULL);

	size_x = rect->dx + rect->width > info->var.xres
			? info->var.xres - rect->dx : rect->width;

	size_y = rect->dy + rect->height > info->var.yres
			? info->var.yres - rect->dy : rect->height;

	base = (char *)info->screen_base;
	assert(base != NULL);

	for (j = 0; j < size_y; ++j) {
		fb_memset(base + (rect->dx + (j + rect->dy) * info->var.xres) * info->var.bits_per_pixel / 8,
				rect->color, size_x * info->var.bits_per_pixel / 8);
	}
}
