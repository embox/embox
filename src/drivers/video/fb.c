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
#include <limits.h>

#define MODOPS_FB_AMOUNT OPTION_GET(NUMBER, fb_amount)

POOL_DEF(fb_pool, struct fb_info, MODOPS_FB_AMOUNT);
static struct fb_info *registered_fb[MODOPS_FB_AMOUNT];
static unsigned int num_registered_fb = 0;

extern const struct kfile_operations fb_device_ops;

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
	return char_dev_register(&name[0], &fb_device_ops);
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

	if (info->ops->fb_set_par != NULL) {
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
			j = size_y;
			while (j--) {
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
			j = size_y;
			while (j--) {
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

static uint32_t pixel_to_pat(uint32_t bpp, uint32_t pixel) {
	assert(pixel >> bpp == 0);
	return bpp == 1 ? 0xffffffffUL * pixel
			: bpp == 2 ? 0x55555555UL * pixel
			: bpp == 4 ? 0x11111111UL * pixel
			: bpp == 8 ? 0x01010101UL * pixel
			: bpp == 12 ? 0x01001001UL * pixel
			: bpp == 16 ? 0x00010001UL * pixel
			: bpp == 24 ? 0x01000001UL * pixel
			: bpp == 32 ? 0x00000001UL * pixel
			: 0;
}

static void bitfill(uint32_t *dest, uint32_t bitn, uint32_t pat,
		uint32_t loff, uint32_t roff, uint32_t len) {
	uint32_t mask1, mask2;

	assert(dest != NULL);

	if (len == 0) return;

	mask1 = ~(uint32_t)0 << bitn;
	mask2 = ~(~(uint32_t)0 << (bitn + len) % (sizeof(*dest) * CHAR_BIT));

	if (bitn + len <= sizeof(*dest) * CHAR_BIT) {
		mask1 &= mask2 != 0 ? mask2 : mask1;
		fb_writel((pat & mask1) | (fb_readl(dest) & ~mask1), dest);
		return;
	}

	if (mask1 != 0) {
		fb_writel((pat & mask1) | (fb_readl(dest) & ~mask1), dest);
		++dest;
		pat = (pat << loff) | (pat >> roff);
		len -= sizeof(*dest) * CHAR_BIT - bitn;
	}

	len /= sizeof(*dest) * CHAR_BIT;
	while (len-- > 0) {
		fb_writel(pat, dest);
		++dest;
		pat = (pat << loff) | (pat >> roff);
	}

	*dest = (pat & mask2) | (*dest & ~mask2);
}

void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {
	uint32_t width, height, pat_orig, pat, *dest, bitn, loff, roff;

	assert(info != NULL);
	assert(rect != NULL);

	width = rect->dx + rect->width > info->var.xres
			? info->var.xres - rect->dx : rect->width;

	height = rect->dy + rect->height > info->var.yres
			? info->var.yres - rect->dy : rect->height;

	pat_orig = pixel_to_pat(info->var.bits_per_pixel, rect->color);

	assert(info->screen_base != NULL);
	bitn = (uint32_t)info->screen_base % sizeof(*dest);
	dest = (uint32_t *)((uint32_t)info->screen_base - bitn);
	bitn = bitn * CHAR_BIT + (rect->dy * info->var.xres
			+ rect->dx) * info->var.bits_per_pixel;

	roff = sizeof(*dest) * CHAR_BIT % info->var.bits_per_pixel;
	loff = info->var.bits_per_pixel - roff;

	while (height-- > 0) {
		dest += bitn / (sizeof(*dest) * CHAR_BIT);
		bitn %= sizeof(*dest) * CHAR_BIT;

		pat = roff != 0 ? pat_orig : (pat_orig << (bitn % info->var.bits_per_pixel))
				| (pat_orig >> (info->var.bits_per_pixel - bitn % info->var.bits_per_pixel));

		bitfill(dest, bitn, pat, loff, roff, width * info->var.bits_per_pixel);
		bitn += info->var.xres * info->var.bits_per_pixel;
	}
}

void fb_imageblit(struct fb_info *info, const struct fb_image *image) {
	/* TODO it's slow version:) */
	uint32_t i, j;
	struct fb_fillrect rect;

	assert(info != NULL);
	assert(image != NULL);
	assert(image->depth == 1);
	assert(image->width == 8);

	rect.width = rect.height = 1;
	for (j = 0; j < image->height; ++j) {
		rect.dy = image->dy + j * rect.height;
		for (i = 0; i < image->width; ++i) {
			rect.dx = image->dx + i * rect.width;
			rect.color = *(uint8_t *)(image->data + j) & (1 << ((image->width - i - 1)))
					? image->fg_color : image->bg_color;
			fb_fillrect(info, &rect);
		}
	}
}
