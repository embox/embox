/**
 * @file
 *
 * @date Oct 18, 2021
 * @author Anton Bondarev
 */

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>
#include <util/log.h>
#include <kernel/irq.h>

#include <embox/unit.h>

#include <stm32f746xx.h>
#include "stm32cube_custom_lcd.h"

EMBOX_UNIT_INIT(stm32cube_lcd_init);

#define LTDC_IRQ      OPTION_GET(NUMBER,ltdc_irq)
static_assert(LTDC_IRQ == LTDC_IRQn, "");

#define USE_FB_SECTION OPTION_GET(BOOLEAN,use_fb_section)

#if USE_FB_SECTION
#define STM32_FB_SECTION_START OPTION_GET(STRING,fb_section_start)
extern char STM32_FB_SECTION_START;
#else
#define STM32_FB_START  OPTION_GET(NUMBER, fb_base)
#endif

#define STM32_LCD_HEIGHT  OPTION_GET(NUMBER, height)
#define STM32_LCD_WIDTH   OPTION_GET(NUMBER, width)
#define STM32_LCD_BPP     OPTION_GET(NUMBER, bpp)

static_assert(STM32_LCD_BPP == 16 || STM32_LCD_BPP == 32, "");

/* Initialize layer 0 as active for all F7 boards. */
#undef LTDC_ACTIVE_LAYER
#define LTDC_ACTIVE_LAYER 0


extern LTDC_HandleTypeDef hLtdcHandler;
#define hltdc_handler hLtdcHandler

static int stm32cube_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int stm32cube_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));

	var->xres = bsp_lcd_getxsize();
	var->yres = bsp_lcd_getysize();
	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;
	var->bits_per_pixel = STM32_LCD_BPP;

	switch (STM32_LCD_BPP) {
	case 16:
		var->fmt = BGR565;
		break;
	case 32:
		var->fmt = BGRA8888;
		break;
	default:
		log_error("unknown BPP = %d\n", STM32_LCD_BPP);
		return -1;
	}

	return 0;
}

static void stm32cube_lcd_fillrect(struct fb_info *info,
		const struct fb_fillrect *rect) {

	bsp_lcd_settextcolor(rect->color | 0xff000000);
	bsp_lcd_fillrect(rect->dx, rect->dy, rect->width, rect->height);
}

static uint32_t stm32cube_get_image_color(const struct fb_image *image, int num) {
	switch (image->depth) {
	case 1:
		if (image->data[num / 8] & (1 << (8 - num % 8))) {
			return image->fg_color;
		} else {
			return image->bg_color;
		}
	case 16:
		return ((uint16_t *) image->data)[num];
	case 32:
		return ((uint32_t *) image->data)[num];
	default:
		log_error("Unsupported color depth: %d\n", image->depth);
		return image->bg_color;
	}
}

static void stm32cube_lcd_imageblit(struct fb_info *info,
		const struct fb_image *image) {
	int dy = image->dy, dx = image->dx;
	int height = image->height, width = image->width;
	int n = 0;

	for (int j = dy; j < dy + height; j++) {
		for (int i = dx; i < dx + width; i++) {
			bsp_lcd_drawpixel(i, j, stm32cube_get_image_color(image, n));
		}
	}
}

static void ltdc_layer_init(uint16_t LayerIndex, uint32_t FB_Address, int bpp) {
	LCD_LayerCfgTypeDef  Layercfg;

	assert(bpp == 16 || bpp == 32);

	switch (bpp) {
	case 16:
		Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
		break;
	case 32:
		Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
		break;
	default:
		return;
	}

	/* Layer Init */
	Layercfg.WindowX0 = 0;
	Layercfg.WindowX1 = bsp_lcd_getxsize();
	Layercfg.WindowY0 = 0;
	Layercfg.WindowY1 = bsp_lcd_getysize();
	Layercfg.FBStartAdress = FB_Address;
	Layercfg.Alpha = 255;
	Layercfg.Alpha0 = 0;
	Layercfg.Backcolor.Blue = 0;
	Layercfg.Backcolor.Green = 0;
	Layercfg.Backcolor.Red = 0;
	Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	Layercfg.ImageWidth = bsp_lcd_getxsize();
	Layercfg.ImageHeight = bsp_lcd_getysize();

	HAL_LTDC_ConfigLayer(&hltdc_handler, &Layercfg, LayerIndex);
}

static irq_return_t ltdc_irq_handler(unsigned int irq_num, void *dev_id) {
	HAL_LTDC_IRQHandler(&hltdc_handler);

	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(LTDC_IRQ, ltdc_irq_handler, NULL);

static struct fb_ops stm32cube_lcd_ops = {
	.fb_set_var   = stm32cube_lcd_set_var,
	.fb_get_var   = stm32cube_lcd_get_var,
	.fb_fillrect  = stm32cube_lcd_fillrect,
	.fb_imageblit = stm32cube_lcd_imageblit,
};

static int stm32cube_lcd_init(void) {
	char *mmap_base;

	if (bsp_lcd_init(STM32_LCD_HEIGHT, STM32_LCD_WIDTH) != LCD_OK) {
		log_error("Failed to init LCD!");
		return -1;
	}

#if USE_FB_SECTION
	mmap_base = (char *) &STM32_FB_SECTION_START;
#else
	mmap_base = (char *) STM32_FB_START;
#endif

	ltdc_layer_init(LTDC_ACTIVE_LAYER, (unsigned int) mmap_base, STM32_LCD_BPP);

	bsp_lcd_setxsize(STM32_LCD_WIDTH);
	bsp_lcd_setysize(STM32_LCD_HEIGHT);

	bsp_lcd_setbackcolor(LCD_COLOR_WHITE);
	bsp_lcd_settextcolor(LCD_COLOR_BLACK);

	bsp_lcd_selectlayer(LTDC_ACTIVE_LAYER);

	bsp_lcd_clear(LCD_COLOR_BLACK);

	if (0 > irq_attach(LTDC_IRQ, ltdc_irq_handler, 0, NULL, "LTDC")) {
		log_error("irq_attach failed");
		return -1;
	}

	fb_create(&stm32cube_lcd_ops,
			mmap_base,
			STM32_LCD_WIDTH *
			STM32_LCD_HEIGHT /
			8 * STM32_LCD_BPP);

	return 0;
}
