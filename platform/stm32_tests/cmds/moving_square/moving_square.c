#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>

#include <drivers/video/fb.h>

#if defined STM32F746xx
#include "stm32746g_discovery_lcd.h"
extern LTDC_HandleTypeDef hLtdcHandler;
#define hltdc_handler hLtdcHandler
#elif defined STM32F769xx
#include "stm32f769i_discovery_lcd.h"
extern LTDC_HandleTypeDef  hltdc_discovery;
#define hltdc_handler hltdc_discovery
#else
#error Unsupported platform
#endif

#define SQUARE_WIDTH        100
#define SQUARE_MOVING_STEP  5

static uint8_t *fb_buf[2];
static int bpp, width, height, size;
static int fb_buf_idx;
static bool buffer_pending = true;

static void dma2d_fill(uint8_t *dst, uint16_t xsize, uint16_t ysize, uint32_t color) {
	DMA2D_HandleTypeDef hdma2d;

	/*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
	hdma2d.Init.Mode          = DMA2D_R2M;
	hdma2d.Init.ColorMode     = DMA2D_OUTPUT_ARGB8888;
	hdma2d.Init.OutputOffset  = 0;
#if defined STM32F769xx
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
#if defined STM32F769xx
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

	SCB_InvalidateDCache();
}

void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
	if (!buffer_pending) {
		goto out;
	}
	BSP_LCD_SetLayerAddress(0, (uint32_t) fb_buf[fb_buf_idx]);
	fb_buf_idx = (fb_buf_idx + 1) % 2;
	buffer_pending = false;
out:
	HAL_LTDC_ProgramLineEvent(hltdc, height - 1);
}

void moving_square(void) {
	struct fb_info *fb_info;
	int i, x;

	fb_info = fb_lookup(0);
	if (!fb_info) {
		fprintf(stderr, "No framebuffer 0 found\n");
	}

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n",
			fb_info->var.xres, fb_info->var.yres,
			fb_info->var.bits_per_pixel);

	bpp = fb_info->var.bits_per_pixel / 8;
	width = fb_info->var.xres;
	height = fb_info->var.yres;
	size = width * height * bpp;
	
	for (i = 0; i < 2; i++) {
		fb_buf[i] = memalign(64, size);
		if (!fb_buf[i]) {
			fprintf(stderr, "Cannot allocate buffer for screen\n");
			return;
		}
	}

	memset(fb_buf[0], 0xffffffff, size);

	BSP_LCD_SetLayerAddress(0, (uint32_t) fb_buf[0]);

	BSP_LCD_SelectLayer(0);
	BSP_LCD_SetTextColor(0xff00ff00); /* green */
	x = 0;
	BSP_LCD_FillRect(x, 100, SQUARE_WIDTH, SQUARE_WIDTH);

	/* Make two buffers the same. */
	memcpy(fb_buf[1], fb_buf[0], size);

	HAL_LTDC_ProgramLineEvent(&hltdc_handler, height - 1);

	while (1) {
		if (buffer_pending) {
			/* LTDC not handled buffer yet */
			continue;
		}

		dma2d_fill(fb_buf[fb_buf_idx], width, height, 0xffffffff);

		x += SQUARE_MOVING_STEP;
		if (x > width - SQUARE_WIDTH) {
			x = 0;
		}
		BSP_LCD_FillRect(x, 100, SQUARE_WIDTH, SQUARE_WIDTH);

		buffer_pending = true;
	}
}
