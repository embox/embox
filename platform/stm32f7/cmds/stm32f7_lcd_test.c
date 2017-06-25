
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"

#define LCD_FRAMEBUFFER  SDRAM_DEVICE_ADDR

static void lcd_test(void) {
	if (BSP_LCD_Init() != LCD_OK) {
		printf(">>> BSP_LCD_Init failed\n");
	}

	/* Initialize the LCD Layers */
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAMEBUFFER);


	  /* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

	/* Display LCD messages */
	BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32F746G BSP", CENTER_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"Embox LCD example", CENTER_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 200, (uint8_t *)"All rights reserved", CENTER_MODE);
}

int main(int argc, char *argv[]) {
	printf("STM32F7 LCD test start\n");
	
	lcd_test();

	while (1) {
	}

	return 0;
}
