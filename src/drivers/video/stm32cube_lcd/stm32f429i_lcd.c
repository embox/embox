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

#include "stm32f429i_discovery_lcd.h"

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

extern LTDC_HandleTypeDef  LtdcHandler;
#define hltdc_handler LtdcHandler

static int stm32cube_lcd_set_var(struct fb_info *info,
        struct fb_var_screeninfo const *var) {
    return 0;
}

static int stm32cube_lcd_get_var(struct fb_info *info,
        struct fb_var_screeninfo *var) {
    memset(var, 0, sizeof(*var));

    var->xres = BSP_LCD_GetXSize();
    var->yres = BSP_LCD_GetYSize();

    var->xres_virtual = var->xres;
    var->yres_virtual = var->yres;

    var->bits_per_pixel = STM32_LCD_BPP;

    var->fmt = BGR565;

    return 0;
}

static void stm32cube_lcd_fillrect(struct fb_info *info,
        const struct fb_fillrect *rect)
{
    BSP_LCD_SetTextColor(rect->color | 0xff000000);
    BSP_LCD_FillRect(rect->dx, rect->dy, rect->width, rect->height);
}

static uint32_t stm32cube_get_image_color(const struct fb_image *image, int num)
{
    if (image->data[num / 8] & (1 << (8 - num % 8))) {
        return image->fg_color;
    } else {
        return image->bg_color;
    }
    return ((uint16_t *) image->data)[num];
}

static void stm32cube_lcd_imageblit(struct fb_info *info,
        const struct fb_image *image)
{
    int dy = image->dy, dx = image->dx;
    int height = image->height, width = image->width;
    int n = 0;

    for (int j = dy; j < dy + height; j++) {
        for (int i = dx; i < dx + width; i++)
        {
            BSP_LCD_DrawPixel(i, j, stm32cube_get_image_color(image, n));
        }
    }
}

static void ltdc_layer_init(uint16_t LayerIndex, uint32_t FB_Address)
{
    LCD_LayerCfgTypeDef  Layercfg;

    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = BSP_LCD_GetXSize();
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = BSP_LCD_GetYSize();

    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

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

static int stm32cube_lcd_init(void)
{
    char *mmap_base;

    if (BSP_LCD_Init() != LCD_OK) {
        log_error("Failed to init LCD!");
        return -1;
    }

#if USE_FB_SECTION
    mmap_base = (char *) &STM32_FB_SECTION_START;
#else
    mmap_base = (char *) STM32_FB_START;
#endif

    ltdc_layer_init(LTDC_ACTIVE_LAYER, (uint32_t) mmap_base);

    /* Enable The LCD */
    BSP_LCD_DisplayOn();

    BSP_LCD_Clear(0x00000000);

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
