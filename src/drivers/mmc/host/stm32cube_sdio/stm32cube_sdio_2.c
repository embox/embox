/**
 * @file
 * @brief 
 * @author Anton Bondarev
 * @version 0.1
 * @date 10.07.2025
 */

#include <util/log.h>

#include <assert.h>

#include <util/macro.h>

#include <kernel/irq.h>

#include <drivers/gpio.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include "stm32cube_definitions.h"

#include <embox/unit.h>
#include <framework/mod/options.h>


EMBOX_UNIT_INIT(stm32cube_sdio_init);

extern const struct mmc_host_ops stm32cube_sdio_ops;

#define SDIO_ID               OPTION_GET(NUMBER,sdio_id)


#define __DMAx_TxRx_CLK_ENABLE            __HAL_RCC_DMA2_CLK_ENABLE
#define SD_DMAx_Tx_CHANNEL                DMA_CHANNEL_11
#define SD_DMAx_Rx_CHANNEL                DMA_CHANNEL_11
#define SD_DMAx_Tx_STREAM                 DMA2_Stream5
#define SD_DMAx_Rx_STREAM                 DMA2_Stream0
#define SD_DMAx_Tx_IRQn                   DMA2_Stream5_IRQn
#define SD_DMAx_Rx_IRQn                   DMA2_Stream0_IRQn


#define SDIO_INSTANCE         MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_REGION_BASE)

#define SDIO_IRQ_NUM          MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_IRQ)
#define SDIO_CLK_ENABLE       MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_CLK_ENABLE)()

#define STM32_DMA_TX_IRQ      MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_IRQ_DMA_RX)
#define STM32_DMA_RX_IRQ      MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_IRQ_DMA_TX)


#define TRANSFER_CLK_DIV      MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_CLK_DEF_TRANSFER_DIV)

#define SDIO_D0_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_PORT)
#define SDIO_D0_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_NR)
#define SDIO_D0_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_AF)
#define SDIO_D1_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_PORT)
#define SDIO_D1_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_NR)
#define SDIO_D1_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_AF)
#define SDIO_D2_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_PORT)
#define SDIO_D2_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_NR)
#define SDIO_D2_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_AF)
#define SDIO_D3_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_PORT)
#define SDIO_D3_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_NR)
#define SDIO_D3_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_AF)

#define SDIO_CK_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_PORT)
#define SDIO_CK_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_NR)
#define SDIO_CK_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_AF)

#define SDIO_CMD_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_PORT)
#define SDIO_CMD_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_NR)
#define SDIO_CMD_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_AF)

extern irq_return_t stm32cube_sdio_dma_tx_irq(unsigned int irq_num, void *dev);
extern irq_return_t stm32cube_sdio_irq_handler(unsigned int irq_num, void *arg);
extern irq_return_t stm32cube_sdio_dma_rx_irq(unsigned int irq_num, void *dev);

static int stm32cube_sdio_hw_init(void) {

	/* SDIO clock enable */
 	SDIO_CLK_ENABLE();

	/*********************/

	gpio_setup_mode(SDIO_D0_PORT, 1 << SDIO_D0_PIN,
		GPIO_MODE_ALT_SET(SDIO_D0_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D1_PORT, 1 << SDIO_D1_PIN,
		GPIO_MODE_ALT_SET(SDIO_D1_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D2_PORT, 1 << SDIO_D2_PIN,
		GPIO_MODE_ALT_SET(SDIO_D2_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D3_PORT, 1 << SDIO_D3_PIN,
		GPIO_MODE_ALT_SET(SDIO_D3_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(SDIO_CK_PORT, 1 << SDIO_CK_PIN,
		GPIO_MODE_ALT_SET(SDIO_CK_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_CMD_PORT, 1 << SDIO_CMD_PIN,
		GPIO_MODE_ALT_SET(SDIO_CMD_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	return 0;

}

static int stm32_sdio_dma_init(SD_HandleTypeDef *phsd) {
	static DMA_HandleTypeDef dma_rx_handle;
	static DMA_HandleTypeDef dma_tx_handle;

	/* Enable DMA2 clocks */
	__DMAx_TxRx_CLK_ENABLE();
	/* Configure DMA Rx parameters */
	dma_rx_handle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
	dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
	dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dma_rx_handle.Init.Mode                = DMA_PFCTRL;
	dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
	dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
	
	dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;

	/* Associate the DMA handle */
	__HAL_LINKDMA(phsd, hdmarx, dma_rx_handle);
	
	/* Deinitialize the stream for new transfer */
	HAL_DMA_DeInit(&dma_rx_handle);
	
	/* Configure the DMA stream */
	HAL_DMA_Init(&dma_rx_handle);
	
	/* Configure DMA Tx parameters */
	dma_tx_handle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
	dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
	dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dma_tx_handle.Init.Mode                = DMA_PFCTRL;
	dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
	dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;

	dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;

	/* Associate the DMA handle */
	__HAL_LINKDMA(phsd, hdmatx, dma_tx_handle);

	/* Deinitialize the stream for new transfer */
	
	HAL_DMA_DeInit(&dma_tx_handle);

	/* Configure the DMA stream */
	HAL_DMA_Init(&dma_tx_handle);

	return 0;
}

static SD_HandleTypeDef hsd;

static int stm32cube_sdio_init(void) {
	int res;

	/* It seems that  we can't check directly
	 * if any card present, so we just assume
	 * that we have a single card number zero */
	struct mmc_host *mmc = mmc_alloc_host();

	mmc->ops = &stm32cube_sdio_ops;
	mmc->priv = &hsd;

	res = irq_attach(STM32_DMA_RX_IRQ, stm32cube_sdio_dma_rx_irq, 0, NULL,
									"stm32_dma_rx_irq");
	if (res != 0) {
		log_error("irq_attach error");
		return -1;
	}

	res = irq_attach(STM32_DMA_TX_IRQ, stm32cube_sdio_dma_tx_irq, 0, NULL,
									"stm32_dma_tx_irq");
	if (res != 0) {
		log_error("irq_attach error");
		return -1;
	}

	res = irq_attach(SDIO_IRQ_NUM, stm32cube_sdio_irq_handler, 0, NULL,
									"stm32_sdmmc_irq");
	if (res != 0) {
		log_error("irq_attach error");
		return -1;
	}

	/* SDMMC2 irq priority should be higher that DMA due to
	 * STM32Cube implementation. */
	irqctrl_set_prio(STM32_DMA_RX_IRQ, 10);
	irqctrl_set_prio(STM32_DMA_TX_IRQ, 10);
	irqctrl_set_prio(SDIO_IRQ_NUM, 11);

	stm32cube_sdio_hw_init();

	/* USER CODE END SDIO_Init 1 */
	hsd.Instance = (void*)((uintptr_t)SDIO_INSTANCE);
	hsd.Init.ClockEdge = CUBE_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = CUBE_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = CUBE_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = CUBE_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = CUBE_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = TRANSFER_CLK_DIV;
	/* USER CODE BEGIN SDIO_Init 2 */

	stm32_sdio_dma_init(&hsd);

	// Общая настройка.
	// https://imax9.narod.ru/publs/F407les04.html
  
	// Обход бага с 4-бита инитом.
	// https://community.st.com/t5/stm32cubemx-mcus/sdio-interface-not-working-in-4bits-with-stm32f4-firmware/td-p/591776
  
	if (HAL_SD_Init(&hsd) != HAL_OK)
	{
    	return 0;
	}

	if (HAL_SD_ConfigWideBusOperation(&hsd, CUBE_BUS_WIDE_4B) != HAL_OK) {
		return 0;
	}


	mmc_scan(mmc);

	return 0;
}

STATIC_IRQ_ATTACH(STM32_DMA_RX_IRQ, stm32cube_sdio_dma_rx_irq,  &hsd);
STATIC_IRQ_ATTACH(STM32_DMA_TX_IRQ, stm32cube_sdio_dma_tx_irq,  &hsd);
STATIC_IRQ_ATTACH(SDIO_IRQ_NUM, stm32cube_sdio_irq_handler, &hsd);