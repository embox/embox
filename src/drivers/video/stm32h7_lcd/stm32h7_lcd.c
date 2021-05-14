/**
 * @file
 * @brief
 *
 * @date 16.07.15
 * @author
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
EMBOX_UNIT_INIT(stm32f7_lcd_init);

#include "stm32h745i_discovery_lcd.h"


#define LTDC_IRQ      OPTION_GET(NUMBER,ltdc_irq)
static_assert(LTDC_IRQ == LTDC_IRQn);

#define USE_FB_SECTION OPTION_GET(BOOLEAN,use_fb_section)

/* Initialize layer 0 as active for all F7 boards. */
#undef LTDC_ACTIVE_LAYER
#define LTDC_ACTIVE_LAYER 0



#if USE_FB_SECTION
#define STM32_FB_SECTION_START OPTION_GET(STRING,fb_section_start)
extern char STM32_FB_SECTION_START;
#else
#define STM32_FB_START  OPTION_GET(NUMBER, fb_base)
#endif

#define STM32_LCD_HEIGHT  OPTION_GET(NUMBER, height)
#define STM32_LCD_WIDTH   OPTION_GET(NUMBER, width)
#define STM32_LCD_BPP     OPTION_GET(NUMBER, bpp)

extern LTDC_HandleTypeDef hlcd_ltdc;

static int stm32f7_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int stm32f7_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));

	BSP_LCD_GetXSize(0, &var->xres);
	BSP_LCD_GetYSize(0, &var->yres);
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
		log_error("stm32f7_lcd_get_var unknown BPP = %d\n", STM32_LCD_BPP);
		return -1;
	}

	return 0;
}

static void stm32f7_lcd_fillrect(struct fb_info *info,
		const struct fb_fillrect *rect) {
	//BSP_LCD_SetTextColor(rect->color | 0xff000000);
	BSP_LCD_FillRect(0, rect->dx, rect->dy, rect->width, rect->height, rect->color | 0xff000000);
}

static uint32_t stm32h7_get_image_color(const struct fb_image *image, int num) {
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

static void stm32f7_lcd_imageblit(struct fb_info *info,
		const struct fb_image *image) {
	int dy = image->dy, dx = image->dx;
	int height = image->height, width = image->width;
	int n = 0;

	for (int j = dy; j < dy + height; j++) {
		for (int i = dx; i < dx + width; i++) {
			BSP_LCD_WritePixel(0, i, j, stm32h7_get_image_color(image, n));
		}
	}
}

static void ltdc_layer_init(uint16_t LayerIndex, uint32_t FB_Address, int bpp) {
#if 0
	LTDC_LayerCfgTypeDef  Layercfg;

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
	BSP_LCD_GetXSize(0, &Layercfg.WindowX1);
	Layercfg.WindowY0 = 0;
	BSP_LCD_GetYSize(0, &Layercfg.WindowY1);
	Layercfg.FBStartAdress = FB_Address;
	Layercfg.Alpha = 255;
	Layercfg.Alpha0 = 0;
	Layercfg.Backcolor.Blue = 0;
	Layercfg.Backcolor.Green = 0;
	Layercfg.Backcolor.Red = 0;
	Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	Layercfg.ImageWidth = Layercfg.WindowX1;
	Layercfg.ImageHeight = Layercfg.WindowY1;

	HAL_LTDC_ConfigLayer(&hlcd_ltdc, &Layercfg, LayerIndex);
#endif
}

static irq_return_t ltdc_irq_handler(unsigned int irq_num, void *dev_id) {
	HAL_LTDC_IRQHandler(&hlcd_ltdc);

	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(LTDC_IRQ, ltdc_irq_handler, NULL);

static struct fb_ops stm32f7_lcd_ops = {
	.fb_set_var   = stm32f7_lcd_set_var,
	.fb_get_var   = stm32f7_lcd_get_var,
	.fb_fillrect  = stm32f7_lcd_fillrect,
	.fb_imageblit = stm32f7_lcd_imageblit,
};

static int stm32f7_lcd_init(void) {
	char *mmap_base;

#if STM32_LCD_BPP == 16
	/* Initialize the LCD */
	BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LTDC_PIXEL_FORMAT_RGB565, STM32_LCD_WIDTH, STM32_LCD_HEIGHT);
#else
	BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LTDC_PIXEL_FORMAT_ARGB8888, STM32_LCD_WIDTH, STM32_LCD_HEIGHT);
#endif

	BSP_LCD_SetBrightness(0, 100);
	BSP_LCD_DisplayOn(0);

    BSP_LCD_SetLayerVisible(0, 1, DISABLE);
    BSP_LCD_SetLayerVisible(0, 0, ENABLE);



	if (0 > irq_attach(LTDC_IRQ, ltdc_irq_handler, 0, NULL, "LTDC")) {
		log_error("irq_attach failed");
		return -1;
	}

#if USE_FB_SECTION
	mmap_base = (char *) &STM32_FB_SECTION_START;
#else
	mmap_base = (char *) STM32_FB_START;
#endif


#if STM32_LCD_BPP == 16
	ltdc_layer_init(LTDC_ACTIVE_LAYER, (unsigned int) mmap_base, 16);
#elif STM32_LCD_BPP == 32
	ltdc_layer_init(LTDC_ACTIVE_LAYER, (unsigned int) mmap_base, 32);
#else
	#error Unsupported STM32_LCD_BPP value
#endif

	fb_create(&stm32f7_lcd_ops,
			mmap_base,
			STM32_LCD_WIDTH *
			STM32_LCD_HEIGHT /
			8 * STM32_LCD_BPP);

	memset(mmap_base, 0, STM32_LCD_WIDTH * STM32_LCD_HEIGHT * 4);

	return 0;
}
