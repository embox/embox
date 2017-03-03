
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"

/** 
  * @brief  RK043FN48H Timing  
  */
#define  RK043FN48H_HSYNC            ((uint16_t)41)   /* Horizontal synchronization */
#define  RK043FN48H_HBP              ((uint16_t)13)   /* Horizontal back porch      */
#define  RK043FN48H_HFP              ((uint16_t)32)   /* Horizontal front porch     */
#define  RK043FN48H_VSYNC            ((uint16_t)10)   /* Vertical synchronization   */
#define  RK043FN48H_VBP              ((uint16_t)2)    /* Vertical back porch        */
#define  RK043FN48H_VFP              ((uint16_t)2)    /* Vertical front porch       */

#define DISPLAY_WIDTH    480
#define DISPLAY_HEIGHT   272

/** 
  * @brief  RK043FN48H frequency divider  
  */
#define  RK043FN48H_FREQUENCY_DIVIDER    5            /* LCD Frequency divider      */

static uint32_t LCD_FRAMEBUFFER;

static void LCD_Config(void)
{ 
  static LTDC_HandleTypeDef hltdc_F;
  LTDC_LayerCfgTypeDef      pLayerCfg;
  
  printf(">>>>> LCD_Config start\n");

  /* LTDC Initialization -------------------------------------------------------*/
  
  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  /* Initialize the vertical synchronization polarity as active low */ 
  hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  /* Initialize the data enable polarity as active low */ 
  hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL; 
  /* Initialize the pixel clock polarity as input pixel clock */  
  hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  
  /* The RK043FN48H LCD 480x272 is selected */
  /* Timing Configuration */
  hltdc_F.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
  hltdc_F.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
  hltdc_F.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hltdc_F.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hltdc_F.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hltdc_F.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hltdc_F.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
  hltdc_F.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);
  
  /* Configure R,G,B component values for LCD background color : all black background */
  hltdc_F.Init.Backcolor.Blue = 0;
  hltdc_F.Init.Backcolor.Green = 0;
  hltdc_F.Init.Backcolor.Red = 0;

  hltdc_F.Instance = LTDC;
  
/* Layer1 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */ 
  /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 43 
     Vertical start   = vertical synchronization + vertical back porch     = 12
     Horizontal stop = Horizontal start + window width -1 = 43 + 480 -1 
     Vertical stop   = Vertical start + window height -1  = 12 + 272 -1      */
  pLayerCfg.WindowX0 = 0;
  //pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowX1 = DISPLAY_WIDTH;
  pLayerCfg.WindowY0 = 0;
  //pLayerCfg.WindowY1 = 272;
  pLayerCfg.WindowY1 = DISPLAY_HEIGHT;
  
  /* Pixel Format configuration*/ 
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  
  /* Start Address configuration : frame buffer is located at FLASH memory */
  pLayerCfg.FBStartAdress = (uint32_t)LCD_FRAMEBUFFER;
  
  /* Alpha constant (255 == totally opaque) */
  pLayerCfg.Alpha = 255;
  
  /* Default Color configuration (configure A,R,G,B component values) : no background color */
  pLayerCfg.Alpha0 = 0; /* fully transparent */
  pLayerCfg.Backcolor.Blue = 50;
  pLayerCfg.Backcolor.Green = 50;
  pLayerCfg.Backcolor.Red = 50;
  
  /* Configure blending factors */
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  
  /* Configure the number of lines and number of pixels per line */
  pLayerCfg.ImageWidth  = DISPLAY_WIDTH;
  pLayerCfg.ImageHeight = DISPLAY_HEIGHT;
  
  /* Configure the LTDC */  
  if(HAL_LTDC_Init(&hltdc_F) != HAL_OK)
  {
	printf(">>>> HAL_LTDC_Init error!\n");
    /* Initialization Error */
  }
    
  /* Configure the Layer*/
  if(HAL_LTDC_ConfigLayer(&hltdc_F, &pLayerCfg, 1) != HAL_OK)
  {
	printf(">>>> HAL_LTDC_ConfigLayer error!\n");
    /* Initialization Error */
  }  
}

static void init_lcd() {
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	LCD_Config();
}

int main(int argc, char *argv[]) {
	int i, j;

	printf("STM32F7 LTDC test start\n");

	if (BSP_SDRAM_Init() != SDRAM_OK) {
		printf(">>> BSP_SDRAM_Init failed\n");
	}

	LCD_FRAMEBUFFER = SDRAM_DEVICE_ADDR;

	init_lcd();

	for (i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
		*(volatile uint16_t*)(LCD_FRAMEBUFFER + 2 * i) = 0xFFFF;
	}

	for (j = 0; j < DISPLAY_HEIGHT / 2; j++) {
		for (i = 0; i < DISPLAY_WIDTH / 2; i++) {
			*(volatile uint16_t*)(LCD_FRAMEBUFFER + 2 * (j * DISPLAY_WIDTH + i)) = 0xEB20;
		}
	}


	return 0;
}
