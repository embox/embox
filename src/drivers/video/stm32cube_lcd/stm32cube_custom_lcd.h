/**
 * @file
 *
 * @date Oct 19, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_VIDEO_STM32CUBE_LCD_STM32CUBE_CUSTOM_LCD_H_
#define SRC_DRIVERS_VIDEO_STM32CUBE_LCD_STM32CUBE_CUSTOM_LCD_H_

#include <stdint.h>

/* Include SDRAM Driver */
#include "stm32746g_discovery_sdram.h"

#include "stm32746g_discovery.h"


typedef struct
{
  uint32_t TextColor;
  uint32_t BackColor;
  void    *pFont;
}LCD_DrawPropTypeDef;

typedef struct
{
  int16_t X;
  int16_t Y;
}Point, * pPoint;

/**
  * @brief  Line mode structures definition
  */
typedef enum
{
  CENTER_MODE             = 0x01,    /* Center mode */
  RIGHT_MODE              = 0x02,    /* Right mode  */
  LEFT_MODE               = 0x03     /* Left mode   */
}Text_AlignModeTypdef;

/**
  * @}
  */

/** @defgroup STM32746G_DISCOVERY_LCD_Exported_Constants STM32746G_DISCOVERY_LCD Exported Constants
  * @{
  */
#define MAX_LAYER_NUMBER       ((uint32_t)2)

#define LCD_LayerCfgTypeDef    LTDC_LayerCfgTypeDef

#define LTDC_ACTIVE_LAYER	     ((uint32_t)1) /* Layer 1 */
/**
  * @brief  LCD status structure definition
  */
#define LCD_OK                 ((uint8_t)0x00)
#define LCD_ERROR              ((uint8_t)0x01)
#define LCD_TIMEOUT            ((uint8_t)0x02)

/**
  * @brief  LCD FB_StartAddress
  */
#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0000000)

/**
  * @brief  LCD color
  */
#define LCD_COLOR_BLUE          ((uint32_t)0xFF0000FF)
#define LCD_COLOR_GREEN         ((uint32_t)0xFF00FF00)
#define LCD_COLOR_RED           ((uint32_t)0xFFFF0000)
#define LCD_COLOR_CYAN          ((uint32_t)0xFF00FFFF)
#define LCD_COLOR_MAGENTA       ((uint32_t)0xFFFF00FF)
#define LCD_COLOR_YELLOW        ((uint32_t)0xFFFFFF00)
#define LCD_COLOR_LIGHTBLUE     ((uint32_t)0xFF8080FF)
#define LCD_COLOR_LIGHTGREEN    ((uint32_t)0xFF80FF80)
#define LCD_COLOR_LIGHTRED      ((uint32_t)0xFFFF8080)
#define LCD_COLOR_LIGHTCYAN     ((uint32_t)0xFF80FFFF)
#define LCD_COLOR_LIGHTMAGENTA  ((uint32_t)0xFFFF80FF)
#define LCD_COLOR_LIGHTYELLOW   ((uint32_t)0xFFFFFF80)
#define LCD_COLOR_DARKBLUE      ((uint32_t)0xFF000080)
#define LCD_COLOR_DARKGREEN     ((uint32_t)0xFF008000)
#define LCD_COLOR_DARKRED       ((uint32_t)0xFF800000)
#define LCD_COLOR_DARKCYAN      ((uint32_t)0xFF008080)
#define LCD_COLOR_DARKMAGENTA   ((uint32_t)0xFF800080)
#define LCD_COLOR_DARKYELLOW    ((uint32_t)0xFF808000)
#define LCD_COLOR_WHITE         ((uint32_t)0xFFFFFFFF)
#define LCD_COLOR_LIGHTGRAY     ((uint32_t)0xFFD3D3D3)
#define LCD_COLOR_GRAY          ((uint32_t)0xFF808080)
#define LCD_COLOR_DARKGRAY      ((uint32_t)0xFF404040)
#define LCD_COLOR_BLACK         ((uint32_t)0xFF000000)
#define LCD_COLOR_BROWN         ((uint32_t)0xFFA52A2A)
#define LCD_COLOR_ORANGE        ((uint32_t)0xFFFFA500)
#define LCD_COLOR_TRANSPARENT   ((uint32_t)0xFF000000)

/**
  * @brief LCD default font
  */
#define LCD_DEFAULT_FONT        Font24

/**
  * @brief  LCD Reload Types
  */
#define LCD_RELOAD_IMMEDIATE               ((uint32_t)LTDC_SRCR_IMR)
#define LCD_RELOAD_VERTICAL_BLANKING       ((uint32_t)LTDC_SRCR_VBR)


/**
  * @brief LCD special pins
  */
/* Display enable pin */
#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()

/* Backlight control pin */
#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()



extern uint8_t bsp_lcd_init(uint16_t height, uint16_t width);
extern uint32_t bsp_lcd_getxsize(void);
extern uint32_t bsp_lcd_getysize(void);
extern void bsp_lcd_setxsize(uint32_t imageWidthPixels);
extern void bsp_lcd_setysize(uint32_t imageHeightPixels);
extern void bsp_lcd_drawpixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code);
extern void bsp_lcd_settextcolor(uint32_t Color);
extern uint32_t bsp_lcd_gettextcolor(void);
extern void bsp_lcd_setbackcolor(uint32_t Color);
extern uint32_t BSP_LCD_GetBackColor(void);
extern void bsp_lcd_selectlayer(uint32_t LayerIndex);
extern void bsp_lcd_clear(uint32_t Color);
extern void bsp_lcd_fillrect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);

#endif /* SRC_DRIVERS_VIDEO_STM32CUBE_LCD_STM32CUBE_CUSTOM_LCD_H_ */
