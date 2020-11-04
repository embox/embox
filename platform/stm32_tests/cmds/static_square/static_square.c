#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
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

static uint8_t *fb_buf[2];
static int bpp, width, height, size;
static int fb_buf_idx;
static int swaps;

void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
	swaps++;
	BSP_LCD_SetLayerAddress(0, (uint32_t) fb_buf[fb_buf_idx]);
	fb_buf_idx = (fb_buf_idx + 1) % 2; 
	HAL_LTDC_ProgramLineEvent(hltdc, height - 1);
}

void static_square(void) {
	struct fb_info *fb_info;
	int i;

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
	BSP_LCD_FillRect(100, 100, 150, 150);

	/* Make two buffers the same. */
	memcpy(fb_buf[1], fb_buf[0], size);

	HAL_LTDC_ProgramLineEvent(&hltdc_handler, height - 1);

	while (1) {
		sleep(1);
		printf("swaps = %d\n", swaps);
	}
}
