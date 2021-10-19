/**
 * @file
 *
 * @date Oct 18, 2021
 * @author Anton Bondarev
 */

#include <stdint.h>

#include "stm32cube_custom_lcd.h"

#include <framework/mod/options.h>

#define  RK043FN48H_HSYNC            ((uint16_t)OPTION_GET(NUMBER, vsync))   /* Horizontal synchronization */
#define  RK043FN48H_HBP              ((uint16_t)OPTION_GET(NUMBER, hbp))   /* Horizontal back porch      */
#define  RK043FN48H_HFP              ((uint16_t)OPTION_GET(NUMBER, hfp))   /* Horizontal front porch     */
#define  RK043FN48H_VSYNC            ((uint16_t)OPTION_GET(NUMBER, vsync))   /* Vertical synchronization   */
#define  RK043FN48H_VBP              ((uint16_t)OPTION_GET(NUMBER, vbp))    /* Vertical back porch        */
#define  RK043FN48H_VFP              ((uint16_t)OPTION_GET(NUMBER, vfp))    /* Vertical front porch       */

#define  RK043FN48H_FREQUENCY_DIVIDER    5            /* LCD Frequency divider      */

LTDC_HandleTypeDef  hLtdcHandler;
static DMA2D_HandleTypeDef hDma2dHandler;

/* Default LCD configuration with LCD Layer 1 */
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

/**
  * @brief  Initializes the LTDC MSP.
  * @param  hltdc: LTDC handle
  * @param  Params
  * @retval None
  */
__weak void BSP_LCD_MspInit(LTDC_HandleTypeDef *hltdc, void *Params)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable the LTDC and DMA2D clocks */
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  LCD_DISP_GPIO_CLK_ENABLE();
  LCD_BL_CTRL_GPIO_CLK_ENABLE();

  /*** LTDC Pins configuration ***/
  /* GPIOE configuration */
  gpio_init_structure.Pin       = GPIO_PIN_4;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin       = GPIO_PIN_12;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /* GPIOI LTDC alternate configuration */
  gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  /* GPIOJ configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

  /* GPIOK configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOK, &gpio_init_structure);

  /* LCD_DISP GPIO configuration */
  gpio_init_structure.Pin       = LCD_DISP_PIN;     /* LCD_DISP pin has to be manually controlled */
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

  /* LCD_BL_CTRL GPIO configuration */
  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;  /* LCD_BL_CTRL pin has to be manually controlled */
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  Clock Config.
  * @param  hltdc: LTDC handle
  * @param  Params
  * @note   This API is called by BSP_LCD_Init()
  *         Being __weak it can be overwritten by the application
  * @retval None
  */
__weak void BSP_LCD_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  /* RK043FN48H LCD clock configuration */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
  periph_clk_init_struct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
  periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}

/**
  * @brief  Initializes the LCD.
  * @retval LCD state
  */
uint8_t bsp_lcd_init(uint16_t height, uint16_t width)
{
  /* Timing Configuration */
  hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
  hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
  hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveH = (height + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveW = (width + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.TotalHeigh = (height + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
  hLtdcHandler.Init.TotalWidth = (width + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

  /* LCD clock configuration */
  BSP_LCD_ClockConfig(&hLtdcHandler, NULL);

  /* Initialize the LCD pixel width and pixel height */
  hLtdcHandler.LayerCfg->ImageWidth  = width;
  hLtdcHandler.LayerCfg->ImageHeight = height;

  /* Background value */
  hLtdcHandler.Init.Backcolor.Blue = 0;
  hLtdcHandler.Init.Backcolor.Green = 0;
  hLtdcHandler.Init.Backcolor.Red = 0;

  /* Polarity */
  hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hLtdcHandler.Instance = LTDC;

  if(HAL_LTDC_GetState(&hLtdcHandler) == HAL_LTDC_STATE_RESET)
  {
    /* Initialize the LCD Msp: this __weak function can be rewritten by the application */
    BSP_LCD_MspInit(&hLtdcHandler, NULL);
  }
  HAL_LTDC_Init(&hLtdcHandler);

  /* Assert display enable LCD_DISP pin */
  HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

  /* Assert backlight LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

#if !defined(DATA_IN_ExtSDRAM)
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();
#endif

  /* Initialize the font */
//  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  return LCD_OK;
}


/**
  * @brief  Gets the LCD X size.
  * @retval Used LCD X size
  */
uint32_t bsp_lcd_getxsize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  Gets the LCD Y size.
  * @retval Used LCD Y size
  */
uint32_t bsp_lcd_getysize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight;
}

/**
  * @brief  Set the LCD X size.
  * @param  imageWidthPixels : image width in pixels unit
  * @retval None
  */
void bsp_lcd_setxsize(uint32_t imageWidthPixels)
{
  hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  Set the LCD Y size.
  * @param  imageHeightPixels : image height in lines unit
  * @retval None
  */
void bsp_lcd_setysize(uint32_t imageHeightPixels)
{
  hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}

/**
  * @brief  Draws a pixel on LCD.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  RGB_Code: Pixel color in ARGB mode (8-8-8-8)
  * @retval None
  */
void bsp_lcd_drawpixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
  /* Write data value to all SDRAM memory */
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    *(__IO uint16_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*bsp_lcd_getxsize() + Xpos))) = (uint16_t)RGB_Code;
  }
  else
  { /* ARGB8888 format */
    *(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*bsp_lcd_getxsize() + Xpos))) = RGB_Code;
  }
}


/**
  * @brief  Sets the LCD text color.
  * @param  Color: Text color code ARGB(8-8-8-8)
  * @retval None
  */
void bsp_lcd_settextcolor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  Gets the LCD text color.
  * @retval Used text color.
  */
uint32_t bsp_lcd_gettextcolor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  Sets the LCD background color.
  * @param  Color: Layer background color code ARGB(8-8-8-8)
  * @retval None
  */
void bsp_lcd_setbackcolor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  Gets the LCD background color.
  * @retval Used background colour
  */
uint32_t BSP_LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
  * @brief  Selects the LCD Layer.
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
void bsp_lcd_selectlayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
}


static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  /* Register to memory mode with ARGB8888 as color Mode */
  hDma2dHandler.Init.Mode         = DMA2D_R2M;
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    hDma2dHandler.Init.ColorMode    = DMA2D_RGB565;
  }
  else
  { /* ARGB8888 format */
    hDma2dHandler.Init.ColorMode    = DMA2D_ARGB8888;
  }
  hDma2dHandler.Init.OutputOffset = OffLine;

  hDma2dHandler.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hDma2dHandler) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hDma2dHandler, LayerIndex) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hDma2dHandler, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hDma2dHandler, 10);
      }
    }
  }
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: Color of the background
  * @retval None
  */
void bsp_lcd_clear(uint32_t Color)
{
  /* Clear the LCD */
  LL_FillBuffer(ActiveLayer, (uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress), bsp_lcd_getxsize(), bsp_lcd_getysize(), 0, Color);
}


/**
  * @brief  Draws a full rectangle.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Width: Rectangle width
  * @param  Height: Rectangle height
  * @retval None
  */
void bsp_lcd_fillrect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint32_t  x_address = 0;

  /* Set the text color */
  bsp_lcd_settextcolor(DrawProp[ActiveLayer].TextColor);

  /* Get the rectangle start address */
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    x_address = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(bsp_lcd_getxsize()*Ypos + Xpos);
  }
  else
  { /* ARGB8888 format */
    x_address = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(bsp_lcd_getxsize()*Ypos + Xpos);
  }
  /* Fill the rectangle */
  LL_FillBuffer(ActiveLayer, (uint32_t *)x_address, Width, Height, (bsp_lcd_getxsize() - Width), DrawProp[ActiveLayer].TextColor);
}


