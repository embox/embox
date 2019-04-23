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

#define ROP_COPY 0
#define ROP_XOR  1

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
#define FB_MODE_IS_FIRST      16
#define FB_MODE_IS_FROM_VAR   32

enum pix_chan {
	ALPHA_CHAN,
	RED_CHAN,
	GREEN_CHAN,
	BLUE_CHAN,
};

enum pix_fmt {
	UNKNOWN = 0,
	RGB888,
	BGR888,
	RGBA8888,
	BGRA8888,
	RGB565,
	BGR565,
};

extern int pix_fmt_has_alpha(enum pix_fmt fmt);
extern int pix_fmt_bpp(enum pix_fmt fmt);
extern int pix_fmt_chan_bits(enum pix_fmt fmt, enum pix_chan chan);
extern int pix_fmt_chan_get_val(enum pix_fmt fmt, enum pix_chan chan,
		void *data);
extern void pix_fmt_chan_set_val(enum pix_fmt fmt, enum pix_chan chan,
		void *data, int val);
extern int pix_fmt_convert(void *src, void *dst, int n,
		enum pix_fmt in, enum pix_fmt out);

struct fb_info;

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
	enum pix_fmt fmt;
};


struct fb_fix_screeninfo {
	char id[16];               /* identification string eg "TT Builtin" */
	/* Start of frame buffer mem (physical address) */
	unsigned long smem_start;
	uint32_t smem_len;         /* Length of frame buffer mem */
	uint32_t type;             /* see FB_TYPE_*		*/
	uint32_t type_aux;         /* Interleave for interleaved Planes */
	uint32_t visual;           /* see FB_VISUAL_*		*/
	uint16_t xpanstep;         /* zero if no hardware panning  */
	uint16_t ypanstep;         /* zero if no hardware panning  */
	uint16_t ywrapstep;        /* zero if no hardware ywrap    */
	uint32_t line_length;      /* length of a line in bytes    */
	/* Start of Memory Mapped I/O (physical address)*/
	unsigned long mmio_start;
	uint32_t mmio_len;         /* Length of Memory Mapped I/O  */
	/* Indicate to driver which	specific chip/card we have */
	uint32_t accel;
	uint16_t capabilities;     /* see FB_CAP_*			*/
	uint16_t reserved[2];      /* Reserved for future compatibility */
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
};

struct fbcurpos {
	uint16_t x;
	uint16_t y;
};

struct fb_cursor {
	uint16_t enable;
	uint16_t rop;
	const char *mask;
	struct fbcurpos hot;
	struct fb_image image;
};

/**
 * Solely driver's part
 */

struct fb_ops {
	int (*fb_get_var)(struct fb_info *info, struct fb_var_screeninfo *var);
	int (*fb_set_var)(struct fb_info *info, const struct fb_var_screeninfo *var);
	void (*fb_copyarea)(struct fb_info *info, const struct fb_copyarea *area);
	void (*fb_fillrect)(struct fb_info *info, const struct fb_fillrect *rect);
	void (*fb_imageblit)(struct fb_info *info, const struct fb_image *image);
	void (*fb_cursor)(struct fb_info *info, const struct fb_cursor *cursor);
	int (*fb_set_base)(struct fb_info *info, void *new_base);
};

struct fb_info {
	int id; /**< ID, monothonically incremented for each fb */

	struct fb_ops ops; /**< Operations on fb, allowed to be modified by driver */
	char *screen_base; /**< Start of frame buffer */
	size_t screen_size; /**< Maximum lenght of frame buffer */

	struct fb_var_screeninfo var; /**< Current variable settins */
};

extern struct fb_info *fb_create(const struct fb_ops *ops, char *map_base,
		size_t map_size);
extern void fb_delete(struct fb_info *info);

/**
 * Solely application's part
 */

extern struct fb_info *fb_lookup(int id);
extern int fb_set_var(struct fb_info *info, const struct fb_var_screeninfo *var);
extern int fb_get_var(struct fb_info *info, struct fb_var_screeninfo *var);

extern void fb_copyarea(struct fb_info *info, const struct fb_copyarea *area);
extern void fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect);
extern void fb_imageblit(struct fb_info *info, const struct fb_image *image);
extern void fb_cursor(struct fb_info *info, const struct fb_cursor *cursor);


extern int fb_devfs_create(struct fb_info *fbi, char *map_base, size_t map_size);

__END_DECLS

#endif /* DRIVERS_VIDEO_FB_H_ */
