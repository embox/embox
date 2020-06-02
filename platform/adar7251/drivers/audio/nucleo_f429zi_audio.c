/*
 * nucleo_f429zi_audio.c
 *
 *  Created on: Jun 1, 2020
 *      Author: anton
 */


#include <kernel/irq.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_dma.h>

struct sai_device {
	uint32_t buf[4096];
};

static struct sai_device sai_device;

static SAI_HandleTypeDef hsai_BlockA1;
DMA_HandleTypeDef hdma_sai_rx;

/**
  * @brief  Initializes SAI Audio IN MSP.
  * @param  hsai: SAI handle
  * @retval None
  */
static void SAI_AUDIO_IN_MspInit(SAI_HandleTypeDef *hsai, void *Params) {
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable SAI clock */
	__HAL_RCC_SAI1_CLK_ENABLE();

	/* Configure PE-4 (SAI1_FS),
	 * PE-5 (SAI_SCK),
	 * PE-6 (SAI_SD) */
	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF6_SAI1;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

#if 0
  /* Configure the hdma_sai_rx handle parameters */
    hdma_sai_rx.Instance = DMA2_Stream1;
    hdma_sai_rx.Init.Channel             = DMA_CHANNEL_0; /*stream 1 channel 0 */
    hdma_sai_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_sai_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sai_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sai_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* 32 bit */
    hdma_sai_rx.Init.MemDataAlignment    = DMA_PDATAALIGN_WORD; /* 32 bit */
    hdma_sai_rx.Init.Mode                = DMA_CIRCULAR;
    hdma_sai_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_sai_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_sai_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_sai_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;



    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai_rx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_rx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_rx);
#endif

}


static irq_return_t sai_interrupt(unsigned int irq_num, void *dev_id) {
	HAL_DMA_IRQHandler(hsai_BlockA1.hdmarx);
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
  /* Disable SAI peripheral */
  __HAL_SAI_DISABLE(&hsai_BlockA1);
  HAL_SAI_DeInit(&hsai_BlockA1);

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
  /* Enable SAI peripheral */
  __HAL_SAI_ENABLE(&hsai_BlockA1);

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

struct sai_device *sai_init(void) {
	int res;

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	SAI_AUDIO_IN_MspInit(&hsai_BlockA1, NULL);
	MX_SAI1_Init();



	res = irq_attach(DMA2_Stream1_IRQn + 16, sai_interrupt, 0, &sai_device, "");
	if (res < 0) {
		return NULL;
	}
	return &sai_device;
}

int sai_recieve(struct sai_device *sai_dev, uint8_t *buf, int len) {
//	HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t*)buf, len);
	HAL_SAI_Receive(&hsai_BlockA1, (uint8_t*)buf, len, 0xFFFFFFFF);
	return 0;
}

STATIC_IRQ_ATTACH(DMA2_Stream1_IRQn + 16, sai_interrupt, &sai_device);
