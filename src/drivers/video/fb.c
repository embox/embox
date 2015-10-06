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

#include <kernel/thread/sync/mutex.h>

#include <util/dlist.h>
#include <util/math.h>

#include <drivers/char_dev.h>
#include <drivers/video/fb.h>

#include <framework/mod/options.h>
#include <mem/misc/pool.h>

#define MODOPS_FB_AMOUNT OPTION_GET(NUMBER, fb_amount)

struct fb_dev {
	struct dlist_head link;
	struct fb_info info;
};

static int fb_update_current_var(struct fb_info *info);

static void fb_default_copyarea(struct fb_info *info, const struct fb_copyarea *area);
static void fb_default_cursor(struct fb_info *info, const struct fb_cursor *cursor);
static void fb_default_imageblit(struct fb_info *info, const struct fb_image *image);
static void fb_default_fillrect(struct fb_info *info, const struct fb_fillrect *rect);

struct mutex fb_static = MUTEX_INIT_STATIC;
POOL_DEF(fb_pool, struct fb_dev, MODOPS_FB_AMOUNT);
static DLIST_DEFINE(fb_list);
static unsigned int fb_count = 0;

#define fb_readb(addr)       (*(uint8_t *) (addr))
#define fb_readw(addr)       (*(uint16_t *) (addr))
#define fb_readl(addr)       (*(uint32_t *) (addr))
#define fb_writeb(val, addr) (*(uint8_t *) (addr) = (val))
#define fb_writew(val, addr) (*(uint16_t *) (addr) = (val))
#define fb_writel(val, addr) (*(uint32_t *) (addr) = (val))
#define fb_memset            memset
#define fb_memcpy_fromfb     memcpy
#define fb_memcpy_tofb       memcpy

static void fb_ops_fixup(struct fb_ops *ops) {
	if (!ops->fb_copyarea) {
		ops->fb_copyarea = fb_default_copyarea;
	}
	if (!ops->fb_imageblit) {
		ops->fb_imageblit = fb_default_imageblit;
	}
	if (!ops->fb_fillrect) {
		ops->fb_fillrect = fb_default_fillrect;
	}
	if (!ops->fb_cursor) {
		ops->fb_cursor = fb_default_cursor;
	}
}

struct fb_info *fb_create(const struct fb_ops *ops, char *map_base, size_t map_size) {
	struct fb_dev *dev;
	struct fb_info *info;

	mutex_lock(&fb_static);
	{
		dev = pool_alloc(&fb_pool);
		if (dev) {
			info = &dev->info;
			info->id = fb_count++;
			dlist_init(&dev->link);
			dlist_add_next(&dev->link, &fb_list);
		} else{
			info = NULL;
		}
	}
	mutex_unlock(&fb_static);

	if (info) {
		info->screen_base = map_base;
		info->screen_size = map_size;

		memcpy(&info->ops, ops, sizeof(struct fb_ops));
		fb_ops_fixup(&info->ops);


		/* FIXME probably should be in open/start */
		fb_update_current_var(info);
	}

	return info;
}

void fb_delete(struct fb_info *info) {
	struct fb_dev *dev = member_cast_out(info, struct fb_dev, info);

	if (info) {
		mutex_lock(&fb_static);
		{
			dlist_del(&dev->link);
			pool_free(&fb_pool, dev);
		}
		mutex_unlock(&fb_static);
	}
}

struct fb_info *fb_lookup(int id) {
	struct fb_info *ret;

	ret = NULL;
	mutex_lock(&fb_static);
	{
		struct fb_dev *fb_dev;
		dlist_foreach_entry(fb_dev, &fb_list, link) {
			if (fb_dev->info.id == id) {
				ret = &fb_dev->info;
				break;
			}
		}
	}
	mutex_unlock(&fb_static);
	return ret;
}

int fb_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	int ret;

	assert(info != NULL);
	assert(var != NULL);

	if (info->ops.fb_set_var != NULL) {
		ret = info->ops.fb_set_var(info, var);
		if (ret < 0) {
			return ret;
		}
		memcpy(&info->var, var, sizeof(struct fb_var_screeninfo));
	}

	return 0;
}

int fb_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	memcpy(var, &info->var, sizeof(struct fb_var_screeninfo));
	return 0;
}

void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area) {
	info->ops.fb_copyarea(info, area);
}

void fb_cursor(struct fb_info *info, const struct fb_cursor *cursor) {
	info->ops.fb_cursor(info, cursor);
}

void fb_imageblit(struct fb_info *info, const struct fb_image *image) {
	info->ops.fb_imageblit(info, image);
}

void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {
	info->ops.fb_fillrect(info, rect);
}


static int fb_update_current_var(struct fb_info *info) {
	if (info->ops.fb_get_var != NULL) {
		return info->ops.fb_get_var(info, &info->var);
	}
	return -ENOENT;
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

static void fb_default_copyarea(struct fb_info *info, const struct fb_copyarea *area) {
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

static void fb_default_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {
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

static void fb_default_imageblit(struct fb_info *info, const struct fb_image *image) {
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
			info->ops.fb_fillrect(info, &rect);
		}
	}
}

static void fb_default_cursor(struct fb_info *info, const struct fb_cursor *cursor) {
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
			info->ops.fb_fillrect(info, &rect);
		}
	}
}
