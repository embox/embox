/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIDEO_FB_H_
#define DRIVERS_VIDEO_FB_H_

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

__BEGIN_DECLS

struct fb_info;
struct fb_copyarea;
struct fb_fillrect;
struct fb_image;
struct fb_cursor;

struct fb_fix_screeninfo {
	const char *name;
};

struct fb_var_screeninfo {
	uint32_t xres;
	uint32_t yres;
	uint32_t xres_virtual;
	uint32_t yres_virtual;
	uint32_t xoffset;
	uint32_t yoffset;
	uint32_t bits_per_pixel;

	uint32_t pixclock;
	uint32_t left_margin;
	uint32_t right_margin;
	uint32_t upper_margin;
	uint32_t lower_margin;
	uint32_t hsync_len;
	uint32_t vsync_len;
	uint32_t sync;
	uint32_t vmode;
};

struct fb_videomode {
	const char *name;	/* optional */
	uint32_t refresh;		/* optional */
	uint32_t xres;
	uint32_t yres;
	uint32_t pixclock;
	uint32_t left_margin;
	uint32_t right_margin;
	uint32_t upper_margin;
	uint32_t lower_margin;
	uint32_t hsync_len;
	uint32_t vsync_len;
	uint32_t sync;
	uint32_t vmode;
	uint32_t flag;
};


#define FB_SYNC_HOR_HIGH_ACT    1       /* horizontal sync high active  */
#define FB_SYNC_VERT_HIGH_ACT   2       /* vertical sync high active    */
#define FB_SYNC_EXT             4       /* external sync                */
#define FB_SYNC_COMP_HIGH_ACT   8       /* composite sync high active   */


#define FB_VMODE_NONINTERLACED  0       /* non interlaced */
#define FB_VMODE_INTERLACED     1       /* interlaced   */
#define FB_VMODE_DOUBLE         2       /* double scan */
#define FB_VMODE_ODD_FLD_FIRST  4       /* interlaced: top line first */
#define FB_VMODE_MASK           255


#define FB_MODE_IS_UNKNOWN    0
#define FB_MODE_IS_DETAILED   1
#define FB_MODE_IS_STANDARD   2
#define FB_MODE_IS_VESA       4
#define FB_MODE_IS_CALCULATED 8
#define FB_MODE_IS_FIRST	  16
#define FB_MODE_IS_FROM_VAR   32

struct fb_ops {
	size_t (*fb_read)(struct fb_info *info, char *buff, size_t size, size_t *ppos);
	size_t (*fb_write)(struct fb_info *info, const char *buff, size_t size, size_t *ppos);
	int (*fb_ioctl)(struct fb_info *info, int cmd, va_list args);

	int (*fb_check_var)(struct fb_var_screeninfo *var, struct fb_info *info);
	int (*fb_set_par)(struct fb_info *info);

	void (*fb_copyarea)(struct fb_info *info, const struct fb_copyarea *area);
	void (*fb_fillrect)(struct fb_info *info, const struct fb_fillrect *rect);
	void (*fb_imageblit)(struct fb_info *info, const struct fb_image *image);
	void (*fb_cursor)(struct fb_info *info, const struct fb_cursor *cursor);
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

#define ROP_COPY 0
#define ROP_XOR  1

struct fb_fillrect {
	uint32_t dx;
	uint32_t dy;
	uint32_t width;
	uint32_t height;
	uint32_t color;
	uint32_t rop;
};

struct fb_image {
	uint32_t dx;
	uint32_t dy;
	uint32_t width;
	uint32_t height;
	uint32_t fg_color;
	uint32_t bg_color;
	uint32_t depth;
	const char *data;
//	struct fb_cmap cmap;
};

struct fbcurpos {
	uint16_t x;
	uint16_t y;
};

struct fb_cursor {
//	uint16_t set;
	uint16_t enable;
	uint16_t rop;
	const char *mask;
	struct fbcurpos hot;
	struct fb_image image;
};

extern int fb_set_var(struct fb_info *info, struct fb_var_screeninfo *var);

static inline const struct fb_var_screeninfo *fb_get_var(struct fb_info *info) {
	return &info->var;
}

extern int fb_register(struct fb_info *info);
extern int fb_unregister(struct fb_info *info);
extern struct fb_info * fb_lookup(const char *name);

extern void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area);
extern void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect);
extern void fb_imageblit(struct fb_info *info, const struct fb_image *image);
extern void fb_cursor(struct fb_info *info, const struct fb_cursor *cursor);

extern struct fb_info * fb_alloc(void);
extern void fb_release(struct fb_info *info);

extern void fb_videomode_to_var(struct fb_var_screeninfo *var,
		const struct fb_videomode *mode);
extern void fb_var_to_videomode(struct fb_videomode *mode,
		const struct fb_var_screeninfo *var);
extern int fb_try_mode(struct fb_var_screeninfo *var, struct fb_info *info,
		const struct fb_videomode *mode, uint32_t bpp);

#define fb_readb(addr)       (*(uint8_t *) (addr))
#define fb_readw(addr)       (*(uint16_t *) (addr))
#define fb_readl(addr)       (*(uint32_t *) (addr))
#define fb_writeb(val, addr) (*(uint8_t *) (addr) = (val))
#define fb_writew(val, addr) (*(uint16_t *) (addr) = (val))
#define fb_writel(val, addr) (*(uint32_t *) (addr) = (val))
#define fb_memset            memset
#define fb_memcpy_fromfb     memcpy
#define fb_memcpy_tofb       memcpy

struct video_resbpp;
extern const struct fb_videomode *video_fbmode_by_resbpp(const struct video_resbpp *resbpp);

__END_DECLS

#endif /* DRIVERS_VIDEO_FB_H_ */
