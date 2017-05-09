/**
 * @file
 * @brief
 *
 * @date 16.07.15
 * @author
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>
#include <util/log.h>

#include <kernel/printk.h>

#include "stm32746g_discovery_lcd.h"

#include <embox/unit.h>
EMBOX_UNIT_INIT(stm32f7_lcd_init);

#define LCD_FRAMEBUFFER  SDRAM_DEVICE_ADDR

static int stm32f7_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int stm32f7_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));

	var->xres_virtual = var->xres = BSP_LCD_GetXSize();
	var->yres_virtual = var->yres = BSP_LCD_GetYSize();
	var->bits_per_pixel = 24;

	return 0;
}

static void stm32f7_lcd_fillrect(struct fb_info *info,
		const struct fb_fillrect *rect) {
	BSP_LCD_SetTextColor(rect->color | 0xff000000);
	//BSP_LCD_SetTextColor(0xff000000);
	//BSP_LCD_DrawPixel(rect->dx, rect->dy, 0xffff0000);
	//BSP_LCD_DrawPixel(rect->dx, rect->dy, 0xff000000);
	BSP_LCD_FillRect(rect->dx, rect->dy, rect->width, rect->height);
}

#define BG_COLOR  0xFF000000
#define TXT_COLOR 0xFFAAAAAA
static uint32_t _get_color(const uint8_t *data, int num, int depth) {
	switch (depth) {
	case 1:
		if (data[num / 8] & (1 << (8 - num % 8)))
			return TXT_COLOR;
		else
			return BG_COLOR;
	default:
		log_error("Unsupported color depth: %d\n", depth);
		return BG_COLOR;
	}
}

static void stm32f7_lcd_imageblit(struct fb_info *info,
		const struct fb_image *image) {
	int n = 0;

	int dy = image->dy;
	int dx = image->dx;
	int height = image->height;
	int width = image->width;
	int depth = image->depth;

	for (int j = dy; j < dy + height; j++)
		for (int i = dx; i < dx + width; i++)
			BSP_LCD_DrawPixel(i, j, _get_color((uint8_t *)image->data, n++, depth));
}

static struct fb_ops stm32f7_lcd_ops = {
	.fb_set_var   = stm32f7_lcd_set_var,
	.fb_get_var   = stm32f7_lcd_get_var,
	.fb_fillrect  = stm32f7_lcd_fillrect,
	.fb_imageblit = stm32f7_lcd_imageblit,
};

static int stm32f7_lcd_init(void) {
	char *mmap_base = (void*) LCD_FRAMEBUFFER;
	size_t mmap_len = 0;

	if (BSP_LCD_Init() != LCD_OK) {
		log_error("Failed to init LCD!");
		return -1;
	}

	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAMEBUFFER);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	fb_create(&stm32f7_lcd_ops, mmap_base, mmap_len);

	return 0;
}
