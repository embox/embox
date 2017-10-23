/**
 * @file pl110.c
 * @brief Initial implementation for PL110 video controller
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 09.08.2017
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>
#include <util/log.h>

#include <kernel/printk.h>
#include <framework/mod/options.h>
#include <embox/unit.h>
EMBOX_UNIT_INIT(pl110_lcd_init);

#define PL110_BASE       OPTION_GET(NUMBER,base_addr)
#define LCD_FRAMEBUFFER  SDRAM_DEVICE_ADDR

#define PL110_TIMING0    (PL110_BASE + 0x000)
# define PL110_PPL_MASK  0xFC
# define PL110_PPL_OFFT  2
#define PL110_TIMING1    (PL110_BASE + 0x004)
# define PL110_LPP_MASK  0x3FF
#define PL110_TIMING2    (PL110_BASE + 0x008)
#define PL110_TIMING3    (PL110_BASE + 0x00C)
#define PL110_UPBASE     (PL110_BASE + 0x010)
#define PL110_LPBASE     (PL110_BASE + 0x014)
#define PL110_IMSC       (PL110_BASE + 0x018)
#define PL110_CONTROL    (PL110_BASE + 0x01C)
# define PL110_BPP_MASK  0xE
# define PL110_BPP_OFFT  1
# define PL110_LCD_EN    (1 << 0)
# define PL110_LCD_PWR   (1 << 11)
#define PL110_RIS        (PL110_BASE + 0x020)
#define PL110_MIS        (PL110_BASE + 0x024)
#define PL110_ICR        (PL110_BASE + 0x028)
#define PL110_UPCURR     (PL110_BASE + 0x02C)
#define PL110_LPCURR     (PL110_BASE + 0x030)
#define PL110_PALETTE(n) (PL110_BASE + 0x200 + (n) * 4)

#define PL110_MAX_WIDTH    1024
#define PL110_MAX_HEIGHT   1024

static int pl110_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static uint32_t pl110_fb[PL110_MAX_WIDTH * PL110_MAX_HEIGHT]
                        __attribute__ ((aligned (0x4)));

static uint32_t pl110_fb_width, pl110_fb_height;

static int pl110_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	uint32_t tmp;

	memset(var, 0, sizeof(*var));

	tmp = REG32_LOAD(PL110_TIMING0) & PL110_PPL_MASK;
	tmp >>= PL110_PPL_OFFT;
	tmp = 16 * (tmp + 1);
	var->xres_virtual = var->xres = tmp;

	tmp = (REG32_LOAD(PL110_TIMING1) & PL110_LPP_MASK) + 1;
	var->yres_virtual = var->yres = tmp;

	tmp = REG32_LOAD(PL110_CONTROL) & PL110_BPP_MASK;
	tmp >>= PL110_BPP_OFFT;

	if (tmp > 5) {
		log_warning("Reserved BPP value=%d, we "
			    "will try to act as it was 32", tmp);
		tmp = 32;
	} else {
		tmp = 1 << tmp;
	}

	var->bits_per_pixel = tmp;

	return 0;
}

static uint32_t pl110_get_image_color(const struct fb_image *image, int num) {
	switch (image->depth) {
	case 1:
		if (image->data[num / 8] & (1 << (8 - num % 8))) {
			return image->fg_color;
		} else {
			return image->bg_color;
		}
	case 32:
		return ((uint32_t *) image->data)[num];
	default:
		log_error("Unsupported color depth: %d\n", image->depth);
		return image->bg_color;
	}
}

static void pl110_lcd_imageblit(struct fb_info *info,
		const struct fb_image *image) {
	int dy = image->dy, dx = image->dx;
	int height = image->height, width = image->width;
	int n;

	for (int j = dy; j < dy + height; j++) {
		for (int i = dx; i < dx + width; i++) {
			pl110_fb[j * pl110_fb_width + i] =
				pl110_get_image_color(image, n++);
		}
	}
}

static struct fb_ops pl110_lcd_ops = {
	.fb_set_var   = pl110_lcd_set_var,
	.fb_get_var   = pl110_lcd_get_var,
	.fb_imageblit = pl110_lcd_imageblit,
};

static int pl110_lcd_init(void) {
	char *mmap_base = (void *) pl110_fb;
	size_t mmap_len = 0;
	uint32_t tmp;

	tmp = pl110_fb_width = 640;
	tmp /= 16;
	tmp--;
	tmp <<= PL110_PPL_OFFT;
	REG32_STORE(PL110_TIMING0, tmp);

	tmp = pl110_fb_height = 480;
	tmp++;
	REG32_STORE(PL110_TIMING1, tmp);
	REG32_STORE(PL110_UPBASE, (uint32_t) pl110_fb);
	REG32_ORIN(PL110_CONTROL, PL110_LCD_PWR | PL110_LCD_EN);

	REG32_CLEAR(PL110_CONTROL, PL110_BPP_MASK);
	REG32_ORIN(PL110_CONTROL, 5 << PL110_BPP_OFFT);

	fb_create(&pl110_lcd_ops, mmap_base, mmap_len);

	pl110_fb_width  = PL110_MAX_WIDTH;
	pl110_fb_height = PL110_MAX_HEIGHT;

	return 0;
}

static struct periph_memory_desc pl110_mem = {
	.start = PL110_BASE,
	.len   = 0x400,
};

PERIPH_MEMORY_DEFINE(pl110_mem);
