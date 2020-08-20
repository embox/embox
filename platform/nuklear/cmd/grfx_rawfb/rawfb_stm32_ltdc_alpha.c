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

static void dma2d_fill(uint32_t *dst, uint16_t xsize, uint16_t ysize, uint32_t color) {
	DMA2D_HandleTypeDef hdma2d;

	/*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
	hdma2d.Init.Mode          = DMA2D_R2M;
	hdma2d.Init.ColorMode     = DMA2D_OUTPUT_ARGB8888;
	hdma2d.Init.OutputOffset  = 0;
#ifdef STM32F769xx
	hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/
	hdma2d.Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */
#endif

	/*##-2- DMA2D Callbacks Configuration ######################################*/
	hdma2d.XferCpltCallback  = NULL;

	/*##-3- Foreground Configuration ###########################################*/
	hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d.LayerCfg[1].InputAlpha = 0xFF;
	hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
	hdma2d.LayerCfg[1].InputOffset = 0;
#ifdef STM32F769xx
	hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
	hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */
#endif

	hdma2d.Instance          = DMA2D;

	/* DMA2D Initialization */
	if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)  {
		if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) {
			if (HAL_DMA2D_Start(&hdma2d, color, (uint32_t)dst, xsize, ysize) == HAL_OK) {
				/* Polling For DMA transfer */
				HAL_DMA2D_PollForTransfer(&hdma2d, 100);
			}
		}
	}
}

void rawfb_init(struct rawfb_fb_info *rfb) {
	BSP_LCD_LayerDefaultInit(0, (unsigned int) rfb->fb_buf[0]);
	BSP_LCD_LayerDefaultInit(1, (unsigned int) rfb->fb_buf[1]);

	BSP_LCD_SelectLayer(0);

	HAL_LTDC_ProgramLineEvent(&hltdc_handler, rfb->height - 1);
}

void rawfb_clear_screen(struct rawfb_fb_info *rfb) {
	dma2d_fill(rfb->fb_buf[rfb->fb_buf_idx],
	           rfb->width, rfb->height, rfb->clear_color);
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
