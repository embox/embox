/**
 * @file
 * @brief
 *
 * @date 16.07.15
 * @author
 */


//#include "stm32l152c_discovery.h"
#include "stm32l152c_discovery_glass_lcd.h"

#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32cube_lcd_init);

static int stm32cube_lcd_init(void) {
	BSP_LCD_GLASS_Init();
	BSP_LCD_GLASS_DisplayString((uint8_t*)"GL-LCD");
	return 0;
}
