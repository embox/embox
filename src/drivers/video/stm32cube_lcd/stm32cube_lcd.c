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

#if defined STM32F746xx
#include "stm32746g_discovery_lcd.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery_lcd.h"
#elif defined STM32H745xx
#include "stm32h745i_discovery_lcd.h"
#else
#error Unsupported platform
#endif

/* Initialize layer 0 as active for all F7 boards. */
#undef LTDC_ACTIVE_LAYER
#define LTDC_ACTIVE_LAYER 0

#include <embox/unit.h>
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

#if defined STM32F746xx
extern LTDC_HandleTypeDef hLtdcHandler;
#define hltdc_handler hLtdcHandler
#elif defined STM32F769xx
extern LTDC_HandleTypeDef  hltdc_discovery;
#define hltdc_handler hltdc_discovery
#elif defined STM32H745xx
extern LTDC_HandleTypeDef hlcd_ltdc;
#define hltdc_handler hlcd_ltdc
#else
#error Unsupported platform
#endif

static int stm32cube_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int stm32cube_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));

#if defined STM32H745xx
	BSP_LCD_GetXSize(0, &var->xres);
	BSP_LCD_GetYSize(0, &var->yres);
#else
	var->xres = BSP_LCD_GetXSize();
	var->yres = BSP_LCD_GetYSize();
#endif
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
#if defined STM32H745xx
	BSP_LCD_FillRect(0, rect->dx, rect->dy, rect->width, rect->height,
			rect->color | 0xff000000);
#else
	BSP_LCD_SetTextColor(rect->color | 0xff000000);
	BSP_LCD_FillRect(rect->dx, rect->dy, rect->width, rect->height);
#endif
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
#if defined STM32H745xx
			BSP_LCD_WritePixel(0, i, j, stm32cube_get_image_color(image, n));
#else
			BSP_LCD_DrawPixel(i, j, stm32cube_get_image_color(image, n));
#endif
		}
	}
}

static void ltdc_layer_init(uint16_t LayerIndex, uint32_t FB_Address, int bpp) {
#if defined STM32H745xx
#else
	LCD_LayerCfgTypeDef  Layercfg;

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
	Layercfg.WindowX1 = BSP_LCD_GetXSize();
	Layercfg.WindowY0 = 0;
	Layercfg.WindowY1 = BSP_LCD_GetYSize();
	Layercfg.FBStartAdress = FB_Address;
	Layercfg.Alpha = 255;
	Layercfg.Alpha0 = 0;
	Layercfg.Backcolor.Blue = 0;
	Layercfg.Backcolor.Green = 0;
	Layercfg.Backcolor.Red = 0;
	Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	Layercfg.ImageWidth = BSP_LCD_GetXSize();
	Layercfg.ImageHeight = BSP_LCD_GetYSize();

	HAL_LTDC_ConfigLayer(&hltdc_handler, &Layercfg, LayerIndex);
#endif
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

#if defined STM32H745xx
#if STM32_LCD_BPP == 16
	/* Initialize the LCD */
	BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LTDC_PIXEL_FORMAT_RGB565, STM32_LCD_WIDTH, STM32_LCD_HEIGHT);
#else
	BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LTDC_PIXEL_FORMAT_ARGB8888, STM32_LCD_WIDTH, STM32_LCD_HEIGHT);
#endif

#else
	if (BSP_LCD_Init() != LCD_OK) {
		log_error("Failed to init LCD!");
		return -1;
	}

#endif

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

#if defined STM32H745xx
	BSP_LCD_SetBrightness(0, 100);
	BSP_LCD_DisplayOn(0);

	BSP_LCD_SetLayerVisible(0, 1, DISABLE);
	BSP_LCD_SetLayerVisible(0, LTDC_ACTIVE_LAYER, ENABLE);

	memset(mmap_base, 0, STM32_LCD_WIDTH * STM32_LCD_HEIGHT * 4);
#else
	BSP_LCD_SetXSize(STM32_LCD_WIDTH);
	BSP_LCD_SetYSize(STM32_LCD_HEIGHT);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

#ifdef STM32F769xx
	/* STM32746 doesn't support brightness change */
	BSP_LCD_SetBrightness(100);
#endif

	BSP_LCD_Clear(LCD_COLOR_BLACK);
#endif

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
