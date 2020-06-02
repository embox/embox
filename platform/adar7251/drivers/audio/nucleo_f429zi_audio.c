/*
 * nucleo_f429zi_audio.c
 *
 *  Created on: Jun 1, 2020
 *      Author: anton
 */


#include <kernel/irq.h>

#include <stm32f4xx_hal.h>

#if 0
#define AUDIO_SAIx_FS_GPIO_PORT              GPIOG
#define AUDIO_SAIx_FS_AF                     GPIO_AF10_SAI2
#define AUDIO_SAIx_FS_PIN                    GPIO_PIN_9
#define AUDIO_SAIx_SCK_GPIO_PORT             GPIOA
#define AUDIO_SAIx_SCK_AF                    GPIO_AF8_SAI2
#define AUDIO_SAIx_SCK_PIN                   GPIO_PIN_2
#define AUDIO_SAIx_SD_GPIO_PORT              GPIOG
#define AUDIO_SAIx_SD_AF                     GPIO_AF10_SAI2
#define AUDIO_SAIx_SD_PIN                    GPIO_PIN_10
#define AUDIO_SAIx_MCLK_GPIO_PORT            GPIOA
#define AUDIO_SAIx_MCLK_AF                   GPIO_AF10_SAI2
#define AUDIO_SAIx_MCLK_PIN                  GPIO_PIN_1
#endif

struct sai_device {
	uint32_t buf[4096];
};

static struct sai_device sai_device;

static SAI_HandleTypeDef hsai_BlockA1;
DMA_HandleTypeDef hdma_sai1_a;

static irq_return_t sai_interrupt(unsigned int irq_num, void *dev_id) {

	return IRQ_HANDLED;
}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockA1.Init.DataSize = SAI_DATASIZE_32;
  hsai_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockA1.FrameInit.FrameLength = 64;
  hsai_BlockA1.FrameInit.ActiveFrameLength = 1;
  hsai_BlockA1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockA1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockA1.SlotInit.FirstBitOffset = 0;
  hsai_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_32B;
  hsai_BlockA1.SlotInit.SlotNumber = 1;
  hsai_BlockA1.SlotInit.SlotActive = 0x00000000;
  if (HAL_SAI_Init(&hsai_BlockA1) != HAL_OK)
  {
   // Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

int sai_init(void) {
	int res;

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SAI1_Init();

	res = irq_attach(DMA2_Stream1_IRQn + 16, sai_interrupt, 0, &sai_device, "");
	if (res < 0) {
		return res;
	}
	return 0;
}

int sai_recieve(struct sai_device *sai_dev, uint8_t buf, int len) {
	return 0;
}

STATIC_IRQ_ATTACH(DMA2_Stream1_IRQn + 16, sai_interrupt, &sai_device);
