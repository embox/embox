/**
 * @file
 * @brief
 *
 * @date   11.08.2020
 * @author Alexander Kalmuk
 */

#include <drivers/video/fb.h>
#include "rawfb.h"

#if defined STM32F746xx
#include "stm32746g_discovery_lcd.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery_lcd.h"
#else
#error Unsupported platform
#endif

#if defined STM32F746xx
extern LTDC_HandleTypeDef hLtdcHandler;
#define hltdc_handler hLtdcHandler
#elif defined STM32F769xx
extern LTDC_HandleTypeDef  hltdc_discovery;
#define hltdc_handler hltdc_discovery
#else
#error Unsupported platform
#endif

static volatile int ltdc_li_triggered = 0;

void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
	ltdc_li_triggered = 1;
}

void rawfb_init(struct rawfb_fb_info *rfb) {
	BSP_LCD_LayerDefaultInit(0, (uint32_t) rfb->fb_buf[0]);
	BSP_LCD_LayerDefaultInit(1, (uint32_t) rfb->fb_buf[1]);

	HAL_LTDC_ProgramLineEvent(&hltdc_handler, rfb->height - 1);
}

void rawfb_clear_screen(struct rawfb_fb_info *rfb) {
	BSP_LCD_Clear(rfb->clear_color);
}

void rawfb_swap_buffers(struct rawfb_fb_info *rfb) {
	if (rfb->fb_info->var.fmt != BGRA8888) {
		pix_fmt_convert(rfb->fb_buf[rfb->fb_buf_idx],
						rfb->fb_info->screen_base, rfb->width * rfb->height,
						BGRA8888, rfb->fb_info->var.fmt);
	} else {
		/* Here we wait for the next display refresh, and after it happened
		 * swap buffers. */

		ltdc_li_triggered = 0;
		
		HAL_LTDC_ProgramLineEvent(&hltdc_handler, rfb->height - 1);

		while (!ltdc_li_triggered) {
		}

		BSP_LCD_SetTransparency(rfb->fb_buf_idx, 0xff);
	}

	rfb->fb_buf_idx = (rfb->fb_buf_idx + 1) % 2;

	BSP_LCD_SetTransparency(rfb->fb_buf_idx, 0x00);

	BSP_LCD_SelectLayer(rfb->fb_buf_idx);
}
