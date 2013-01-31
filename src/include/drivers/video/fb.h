/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#ifndef LINUX_FB_H_
#define LINUX_FB_H_

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

#define FB_MAX 2

struct fb_info;
struct fb_copyarea;
struct fb_fillrect;

struct fb_fix_screeninfo {
	const char *name;
};

struct fb_var_screeninfo {
	uint32_t xres;
	uint32_t yres;
	uint32_t bits_per_pixel;
};

struct fb_ops {
	size_t (*fb_read)(struct fb_info *info, char *buff, size_t size);
	size_t (*fb_write)(struct fb_info *info, const char *buff, size_t size);
	int (*fb_ioctl)(struct fb_info *info, int cmd, va_list args);

	int (*fb_check_var)(struct fb_var_screeninfo *var, struct fb_info *info);
	int (*fb_set_par)(struct fb_info *info);

	void (*fb_copyarea)(struct fb_info *info, const struct fb_copyarea *region);
	void (*fb_fillrect)(struct fb_info *info, const struct fb_fillrect *rect);
};

struct fb_info {
	uint8_t node;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
	const struct fb_ops *ops;
	uint8_t *screen_base;
	size_t screen_size;
};

struct fb_copyarea {
	uint32_t dx;
	uint32_t dy;
	uint32_t width;
	uint32_t height;
	uint32_t sx;
	uint32_t sy;
};

struct fb_fillrect {
	uint32_t dx;
	uint32_t dy;
	uint32_t width;
	uint32_t height;
	uint32_t color;
};

extern int fb_set_var(struct fb_info *info, struct fb_var_screeninfo *var);

extern int fb_register(struct fb_info *info);
extern int fb_unregister(struct fb_info *info);
extern struct fb_info * fb_lookup(const char *name);

extern void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area);
extern void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect);

extern struct fb_info * fb_alloc(void);
extern void fb_release(struct fb_info *info);

#define fb_readb(addr)       (*(uint8_t *) (addr))
#define fb_readw(addr)       (*(uint16_t *) (addr))
#define fb_readl(addr)       (*(uint32_t *) (addr))
#define fb_writeb(val, addr) (*(uint8_t *) (addr) = (b))
#define fb_writew(val, addr) (*(uint16_t *) (addr) = (b))
#define fb_writel(val, addr) (*(uint32_t *) (addr) = (b))
#define fb_memset            memset
#define fb_memcpy_fromfb     memcpy
#define fb_memcpy_tofb       memcpy

#endif /* LINUX_FB_H_ */
