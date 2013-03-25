/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <util/math.h>
#include <util/dlist.h>

#include <embox/device.h>
#include <drivers/video/fb.h>

#include <framework/mod/options.h>
#include <mem/misc/pool.h>



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

struct fb_list_item {
	struct dlist_head list;
	struct fb_info *fb_info;
};

static DLIST_DEFINE(fb_repo);
POOL_DEF(fb_repo_pool, struct fb_list_item, 0x4);

int fb_register(struct fb_info *info) {
	unsigned int num;
	struct fb_list_item *item;

	assert(info != NULL);

	if (num_registered_fb == sizeof registered_fb / sizeof registered_fb) {
		return -ENOMEM;
	}

	++num_registered_fb;

	for (num = 0; (num < num_registered_fb) && (registered_fb[num] != NULL); ++num);

	registered_fb[num] = info;
	info->node = num;

	if(NULL == (item = pool_alloc(&fb_repo_pool))) {
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&item->list), &fb_repo);

	return ENOERR;
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

int fb_try_mode(struct fb_var_screeninfo *var, struct fb_info *info,
		const struct fb_videomode *mode, uint32_t bpp) {
	fb_videomode_to_var(var, mode);
	var->bits_per_pixel = bpp;

	if (info->ops->fb_check_var != NULL) {
		return info->ops->fb_check_var(var, info);
	}

	return 0;
}

static void bitcpy(uint32_t *dst, uint32_t dstn, uint32_t *src, uint32_t srcn,
		uint32_t len) {
	uint32_t mask1, mask2, loff, roff, lval, rval, left;
	int32_t shift;

	assert(dst != NULL);
	assert(src != NULL);

	if (len == 0) return;

	mask1 = ~(uint32_t)0 << dstn;
	mask2 = ~(~(uint32_t)0 << (dstn + len) % (sizeof(*dst) * CHAR_BIT));
	shift = dstn - srcn;

	if (shift == 0) {
		if (dstn + len <= sizeof(*dst) * CHAR_BIT) {
			mask1 &= mask2 != 0 ? mask2 : mask1;
			fb_writel((fb_readl(src) & mask1) | (fb_readl(dst) & ~mask1), dst);
		}
		else {
			if (mask1 != 0) {
				fb_writel((fb_readl(src) & mask1) | (fb_readl(dst) & ~mask1), dst);
				++dst, ++src;
				len -= sizeof(*dst) * CHAR_BIT - dstn;
			}

			len /= sizeof(*dst) * CHAR_BIT;
			while (len-- != 0) {
				fb_writel(fb_readl(src), dst);
				++dst;
				++src;
			}

			fb_writel((fb_readl(src) & mask2) | (fb_readl(dst) & ~mask2), dst);
		}
	}
	else {
		loff = (uint32_t)-shift % (sizeof(*dst) * CHAR_BIT);
		roff = (uint32_t)shift % (sizeof(*dst) * CHAR_BIT);

		if (dstn + len <= sizeof(*dst) * CHAR_BIT) {
			mask1 &= mask2 != 0 ? mask2 : mask1;
			if (shift > 0)
				fb_writel(((fb_readl(src) >> roff) & mask1) | (fb_readl(dst) & ~mask1), dst);
			else if (srcn + len <= sizeof(*dst) * CHAR_BIT)
				fb_writel(((fb_readl(src) << loff) & mask1) | (fb_readl(dst) & ~mask1), dst);
			else {
				lval = fb_readl(src);
				++src;
				rval = fb_readl(src);
				fb_writel((((lval << loff) | (rval >> roff)) & mask1) | (fb_readl(dst) & ~mask1), dst);
			}
		}
		else {
			lval = fb_readl(src);
			++src;
			if (shift > 0) {
				fb_writel(((lval >> roff) & mask1) | (fb_readl(dst) & ~mask1), dst);
				++dst;
				len -= sizeof(*dst) * CHAR_BIT - dstn;
			}
			else {
				rval = fb_readl(src);
				++src;
				fb_writel((((lval << loff) | (fb_readl(dst) >> roff)) & mask1) | (fb_readl(dst) & ~mask1), dst);
				lval = rval;
				++dst;
				len -= sizeof(*dst) * CHAR_BIT - dstn;
			}

			left = len % (sizeof(*dst) * CHAR_BIT);
			len /= sizeof(*dst) * CHAR_BIT;
			while (len-- != 0) {
				rval = fb_readl(src);
				++src;
				fb_writel((lval << loff) | (rval >> roff), dst);
				++dst;
				lval = rval;
			}

			if (left <= roff)
				fb_writel(((lval << loff) & mask2) | (fb_readl(dst) & ~mask2), dst);
			else {
				rval = fb_readl(src);
				fb_writel((((lval << loff) | (rval >> roff)) & mask2) | (fb_readl(dst) & ~mask2), dst);
			}
		}
	}
}

static void bitcpy_rev(uint32_t *dst, uint32_t dstn, uint32_t *src,
		uint32_t srcn, uint32_t len) {
	uint32_t mask1, mask2, roff, loff, rval, lval, left;
	int32_t shift;

	assert(dst != NULL);
	assert(src != NULL);

	if (len == 0) return;

	dst += (len - 1) / (sizeof(*dst) * CHAR_BIT);
	src += (len - 1) / (sizeof(*src) * CHAR_BIT);
	if ((len - 1) % (sizeof(*dst) * CHAR_BIT) != 0) {
		dstn += (len - 1) % (sizeof(*dst) * CHAR_BIT);
		dst += dstn / (sizeof(*dst) * CHAR_BIT);
		dstn %= sizeof(*dst) * CHAR_BIT;
		srcn += (len - 1) % (sizeof(*src) * CHAR_BIT);
		src += srcn / (sizeof(*src) * CHAR_BIT);
		srcn %= sizeof(*src) * CHAR_BIT;
	}

	mask1 = ~(uint32_t)0 >> (sizeof(*dst) * CHAR_BIT - 1 - dstn);
	mask2 = ~(~(uint32_t)0 >> (sizeof(*dst) * CHAR_BIT - 1 - (dstn - len) % (sizeof(*dst) * CHAR_BIT)));
	shift = dstn - srcn;

	if (shift == 0) {
		if (dstn + 1 >= len) {
			mask1 &= mask2 != 0 ? mask2 : mask1;
			fb_writel((fb_readl(src) & mask1) | (fb_readl(dst) & ~mask1), dst);
		}
		else {
			if (mask1 != 0) {
				fb_writel((fb_readl(src) & mask1) | (fb_readl(dst) & ~mask1), dst);
				--dst;
				--src;
				len -= dstn + 1;
			}

			len /= sizeof(*dst) * CHAR_BIT;
			while (len-- != 0) {
				fb_writel(fb_readl(src), dst);
				--dst;
				--src;
			}

			fb_writel((fb_readl(src) & mask2) | (fb_readl(dst) & ~mask2), dst);
		}
	}
	else {
		roff = (uint32_t)shift % (sizeof(*dst) * CHAR_BIT);
		loff = (uint32_t)-shift % (sizeof(*dst) * CHAR_BIT);

		if (dstn + 1 >= len) {
			mask1 &= mask2 != 0 ? mask2 : mask1;
			if (shift < 0)
				fb_writel(((fb_readl(src) << loff) & mask1) | (fb_readl(dst) & ~mask1), dst);
			else if (srcn + 1 >= len)
				fb_writel(((fb_readl(src) >> roff) & mask1) | (fb_readl(dst) & ~mask1), dst);
			else {
				rval = fb_readl(src);
				--src;
				lval = fb_readl(src);
				fb_writel((((rval >> roff) | (lval << loff)) & mask1) | (fb_readl(dst) & ~mask1), dst);
			}
		}
		else {
			rval = fb_readl(src);
			--src;
			if (shift < 0) {
				fb_writel(((rval << loff) & mask1) | (fb_readl(dst) & ~mask1), dst);
				--dst;
				len -= dstn + 1;
			}
			else {
				lval = fb_readl(src);
				--src;
				fb_writel((((rval >> roff) | (lval << loff)) & mask1) | (fb_readl(dst) & ~mask1), dst);
				rval = lval;
				--dst;
				len -= dstn + 1;
			}

			left = len % (sizeof(*dst) * CHAR_BIT);
			len /= sizeof(*dst) * CHAR_BIT;
			while (len-- != 0) {
				lval = fb_readl(src);
				--src;
				fb_writel((rval >> roff) | (lval << loff), dst);
				--dst;
				rval = lval;
			}

			if (left <= loff)
				fb_writel(((rval >> roff) & mask2) | (fb_readl(dst) & ~mask2), dst);
			else {
				lval = fb_readl(src);
				fb_writel((((rval << roff) | (lval >> loff)) & mask2) | (fb_readl(dst) & ~mask2), dst);
			}
		}
	}
}

void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area) {
	uint32_t width, height, *dst, dstn, *src, srcn;

	assert(info != NULL);
	assert(area != NULL);

	if ((area->dx >= info->var.xres) || (area->dy >= info->var.yres)
			|| (area->sx >= info->var.xres) || (area->sy >= info->var.yres)) return;

	width = min(area->width, info->var.xres - max(area->sx, area->dx));
	height = min(area->height, info->var.yres - max(area->sy, area->dy));

	assert(info->screen_base != NULL);
	dstn = srcn = (uint32_t)info->screen_base % sizeof(*dst);
	dst = src = (uint32_t *)((uint32_t)info->screen_base - dstn);
	dstn = dstn * CHAR_BIT + (area->dy * info->var.xres
			+ area->dx) * info->var.bits_per_pixel;
	srcn = srcn * CHAR_BIT + (area->sy * info->var.xres
			+ area->sx) * info->var.bits_per_pixel;

	if (((area->dy == area->sy) && (area->dx > area->sx))
			|| (area->dy > area->sy)) {
		dstn += height * info->var.xres * info->var.bits_per_pixel;
		srcn += height * info->var.xres * info->var.bits_per_pixel;
		while (height-- != 0) {
			dstn -= info->var.xres * info->var.bits_per_pixel;
			dst += dstn / (sizeof(*dst) * CHAR_BIT);
			dstn %= sizeof(*dst) * CHAR_BIT;
			srcn -= info->var.xres * info->var.bits_per_pixel;
			src += srcn / (sizeof(*src) * CHAR_BIT);
			srcn %= sizeof(*src) * CHAR_BIT;
			bitcpy_rev(dst, dstn, src, srcn, width * info->var.bits_per_pixel);
		}
	}
	else {
		while (height-- != 0) {
			dst += dstn / (sizeof(*dst) * CHAR_BIT);
			dstn %= sizeof(*dst) * CHAR_BIT;
			src += srcn / (sizeof(*src) * CHAR_BIT);
			srcn %= sizeof(*src) * CHAR_BIT;
			bitcpy(dst, dstn, src, srcn, width * info->var.bits_per_pixel);
			dstn += info->var.xres * info->var.bits_per_pixel;
			srcn += info->var.xres * info->var.bits_per_pixel;
		}
	}
}

static uint32_t pixel_to_pat(uint32_t bpp, uint32_t pixel) {
	return bpp == 1 ? 0xffffffffUL * pixel
			: bpp == 2 ? 0x55555555UL * pixel
			: bpp == 4 ? 0x11111111UL * pixel
			: bpp == 8 ? 0x01010101UL * pixel
			: bpp == 12 ? 0x01001001UL * pixel
			: bpp == 16 ? 0x00010001UL * pixel
			: bpp == 24 ? 0x01000001UL * pixel
			: bpp == 32 ? 0x00000001UL * pixel
			: 0xbadffbad;
}

static void bitfill(uint32_t *dst, uint32_t dstn, uint32_t pat,
		uint32_t loff, uint32_t roff, uint32_t len) {
	uint32_t mask1, mask2;

	assert(dst != NULL);

	if (len == 0) return;

	mask1 = ~(uint32_t)0 << dstn;
	mask2 = ~(~(uint32_t)0 << (dstn + len) % (sizeof(*dst) * CHAR_BIT));

	if (dstn + len <= sizeof(*dst) * CHAR_BIT) {
		mask1 &= mask2 != 0 ? mask2 : mask1;
		fb_writel((pat & mask1) | (fb_readl(dst) & ~mask1), dst);
	}
	else {
		if (mask1 != 0) {
			fb_writel((pat & mask1) | (fb_readl(dst) & ~mask1), dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			len -= sizeof(*dst) * CHAR_BIT - dstn;
		}

		len /= sizeof(*dst) * CHAR_BIT;
		while (len-- != 0) {
			fb_writel(pat, dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
		}

		fb_writel((pat & mask2) | (fb_readl(dst) & ~mask2), dst);
	}
}

static void bitfill_rev(uint32_t *dst, uint32_t dstn, uint32_t pat,
		uint32_t loff, uint32_t roff, uint32_t len) {
	uint32_t mask1, mask2;

	assert(dst != NULL);

	if (len == 0) return;

	mask1 = ~(uint32_t)0 << dstn;
	mask2 = ~(~(uint32_t)0 << (dstn + len) % (sizeof(*dst) * CHAR_BIT));

	if (dstn + len <= sizeof(*dst) * CHAR_BIT) {
		mask1 &= mask2 != 0 ? mask2 : mask1;
		fb_writel(((fb_readl(dst) ^ pat) & mask1) | (fb_readl(dst) & ~mask1), dst);
	}
	else {
		if (mask1 != 0) {
			fb_writel(((fb_readl(dst) ^ pat) & mask1) | (fb_readl(dst) & ~mask1), dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			len -= sizeof(*dst) * CHAR_BIT - dstn;
		}

		len /= sizeof(*dst) * CHAR_BIT;
		while (len >= 4) {
			fb_writel(fb_readl(dst) ^ pat, dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			fb_writel(fb_readl(dst) ^ pat, dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			fb_writel(fb_readl(dst) ^ pat, dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			fb_writel(fb_readl(dst) ^ pat, dst);
			++dst;
			pat = (pat << loff) | (pat >> roff);
			len -= 4;
		}
		while (len-- != 0) {
			fb_writel(fb_readl(dst) ^ pat, dst);
			pat = (pat << loff) | (pat >> roff);
		}

		fb_writel(((fb_readl(dst) ^ pat) & mask2) | (fb_readl(dst) & ~mask2), dst);
	}
}

void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {
	uint32_t width, height, pat_orig, pat, *dst, dstn, loff, roff;
	void (*fill_op)(uint32_t *dst, uint32_t dstn, uint32_t pat,
		uint32_t loff, uint32_t roff, uint32_t len);

	assert(info != NULL);
	assert(rect != NULL);

	if ((rect->dx >= info->var.xres) || (rect->dy >= info->var.yres)) return;

	width = min(rect->width, info->var.xres - rect->dx);
	height = min(rect->height, info->var.yres - rect->dy);

	pat_orig = pixel_to_pat(info->var.bits_per_pixel, rect->color);

	assert(info->screen_base != NULL);
	dstn = (uint32_t)info->screen_base % sizeof(*dst);
	dst = (uint32_t *)((uint32_t)info->screen_base - dstn);
	dstn = dstn * CHAR_BIT + (rect->dy * info->var.xres
			+ rect->dx) * info->var.bits_per_pixel;
	roff = sizeof(*dst) * CHAR_BIT % info->var.bits_per_pixel;
	loff = info->var.bits_per_pixel - roff;

	fill_op = rect->rop == ROP_COPY ? &bitfill : &bitfill_rev;

	while (height-- != 0) {
		dst += dstn / (sizeof(*dst) * CHAR_BIT);
		dstn %= sizeof(*dst) * CHAR_BIT;

		pat = roff == 0 ? pat_orig : (pat_orig << (dstn % info->var.bits_per_pixel))
				| (pat_orig >> (info->var.bits_per_pixel - dstn % info->var.bits_per_pixel));

		fill_op(dst, dstn, pat, loff, roff, width * info->var.bits_per_pixel);
		dstn += info->var.xres * info->var.bits_per_pixel;
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
	rect.rop = ROP_COPY;
	for (j = 0; j < image->height; ++j) {
		rect.dy = image->dy + j * rect.height;
		for (i = 0; i < image->width; ++i) {
			rect.dx = image->dx + i * rect.width;
			rect.color = *(uint8_t *)(image->data + j) & (1 << ((image->width - i - 1)))
					? image->fg_color : image->bg_color;
			info->ops->fb_fillrect(info, &rect);
		}
	}
}

void fb_cursor(struct fb_info *info, const struct fb_cursor *cursor) {
	uint32_t i, j;
	struct fb_fillrect rect;

	assert(info != NULL);
	assert(cursor != NULL);

	if (!cursor->enable) return;

	rect.width = rect.height = 1;
	rect.rop = cursor->rop;
	rect.color = cursor->image.fg_color;
	for (j = 0; j < cursor->image.height; ++j) {
		rect.dy = cursor->hot.y * cursor->image.height + j * rect.height;
		for (i = 0; i < cursor->image.width; ++i) {
			rect.dx = cursor->hot.x * cursor->image.width + i * rect.width;
			info->ops->fb_fillrect(info, &rect);
		}
	}
}
