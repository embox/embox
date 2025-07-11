/**
 * @file
 * @brief 
 * @author Anton Bondarev
 * @version 0.1
 * @date 10.07.2025
 */

#include <util/log.h>

#include <assert.h>

#include <kernel/irq.h>

#include <drivers/gpio.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <embox/unit.h>
#include <framework/mod/options.h>




static int stm32cube_sdio_send_cmd(struct mmc_host *host, struct mmc_request *req) {
	SDMMC_CmdInitTypeDef  sdmmc_cmdinit = {0};
	int response;
	SD_HandleTypeDef *phsd;


	phsd = host->priv;

	if (req->cmd.flags & MMC_RSP_PRESENT) {
		if (req->cmd.flags & MMC_RSP_136) {
			response = SDMMC_RESPONSE_LONG;
		}
		else {
			response = SDMMC_RESPONSE_SHORT;
		}
	} else {
		response = SDMMC_RESPONSE_NO;
	}

  
	/* Set Block Size for Card */ 
	sdmmc_cmdinit.Argument         = (uint32_t)req->cmd.arg;
	sdmmc_cmdinit.CmdIndex         = req->cmd.opcode;
	sdmmc_cmdinit.Response         = response;
	sdmmc_cmdinit.WaitForInterrupt = SDMMC_WAIT_NO;
	sdmmc_cmdinit.CPSM             = SDMMC_CPSM_ENABLE;
	(void)SDMMC_SendCommand(phsd->Instance, &sdmmc_cmdinit);
  
	log_debug("send cmd (%d) arg (%x) resp (%d) longresp (%d)", req->cmd.opcode, req->cmd.arg,
		 req->cmd.flags & MMC_RSP_PRESENT, req->cmd.flags & MMC_RSP_136);

	return 0;
}

static void stm32cube_sdio_request(struct mmc_host *host, struct mmc_request *req) {
	uint32_t *buff;
	SD_HandleTypeDef *phsd;

	assert(host);
	assert(req);

	phsd = host->priv;

	if (req->cmd.flags & MMC_DATA_XFER) {
		#if 0
		SDMMC_DataInitTypeDef config = {0};

		/* Configure the SD DPSM (Data Path State Machine) */
		config.DataTimeOut   = SDMMC_DATATIMEOUT;
		config.DataLength    = req->data.blksz * req->data.blocks;
		config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
		if (req->cmd.flags & MMC_DATA_READ) {
			config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
		} else {
			config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
		}
		config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
		config.DPSM          = SDMMC_DPSM_ENABLE;
		(void)SDMMC_ConfigData(phsd->Instance, &config);
#endif
		HAL_SD_ReadBlocks(phsd, (void *) req->data.addr, req->cmd.arg, 1, 10000);
		log_debug("Set datalen %d", req->data.blksz);
		return ;


	}

	stm32cube_sdio_send_cmd(host, req);

	req->cmd.resp[0] = SDMMC_GetResponse(phsd->Instance, SDMMC_RESP1);
	req->cmd.resp[1] = SDMMC_GetResponse(phsd->Instance, SDMMC_RESP2);
	req->cmd.resp[2] = SDMMC_GetResponse(phsd->Instance, SDMMC_RESP3);
	req->cmd.resp[3] = SDMMC_GetResponse(phsd->Instance, SDMMC_RESP4);

	if (req->cmd.flags & MMC_DATA_XFER) {
		int xfer = req->data.blksz * req->data.blocks;

		buff = (void *) req->data.addr;
		log_error("buff=%p", buff);

		while (xfer > 0) {
			if (req->cmd.flags & MMC_DATA_READ) {
				__HAL_SD_CLEAR_FLAG(phsd, SDMMC_STATIC_FLAGS);
				log_debug("reading  %d byte", req->data.blksz);
				/* Get status data */
				while(!__HAL_SD_GET_FLAG(phsd, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DBCKEND))
				{
					if(__HAL_SD_GET_FLAG(phsd, SDMMC_FLAG_RXFIFOHF))
					{
						*buff = SDMMC_ReadFIFO(phsd->Instance);
						buff++;
						xfer -= sizeof(uint32_t);
					}
				}
			} else {
				log_debug("writting  %d byte", req->data.blksz);
			}
		}
	}
}

const struct mmc_host_ops stm32cube_sdio_ops = {
	.request = stm32cube_sdio_request,
};

#if 0



#if 0
#define SDIO_IRQ_NUM          49
#define SDIO_INSTANCE         SDIO
#define SDIO_CLK_ENABLE       __HAL_RCC_SDIO_CLK_ENABLE
#define TRANSFER_CLK_DIV ((uint8_t)0x4)     

#define SDIO_D0_PORT           2 /* PORTC */
#define SDIO_D0_PIN            8
#define SDIO_D0_FUNC           12
#define SDIO_D1_PORT           2 /* PORTC */
#define SDIO_D1_PIN            9
#define SDIO_D1_FUNC           12
#define SDIO_D2_PORT           2 /* PORTC */
#define SDIO_D2_PIN            10
#define SDIO_D2_FUNC           12
#define SDIO_D3_PORT           2 /* PORTC */
#define SDIO_D3_PIN            11
#define SDIO_D3_FUNC           12

#define SDIO_CK_PORT           2 /* PORTC */
#define SDIO_CK_PIN            12
#define SDIO_CK_FUNC           12

#define SDIO_CMD_PORT           3 /* PORTD */
#define SDIO_CMD_PIN            2
#define SDIO_CMD_FUNC           12


#else
#define SDIO_IRQ_NUM          103
#define SDIO_INSTANCE         SDMMC2
#define SDIO_CLK_ENABLE       __HAL_RCC_SDMMC2_CLK_ENABLE

#define TRANSFER_CLK_DIV SDMMC_TRANSFER_CLK_DIV


#define SDIO_D0_PORT           1 /* PORTB */
#define SDIO_D0_PIN            8
#define SDIO_D0_FUNC           10
#define SDIO_D1_PORT           1 /* PORTB */
#define SDIO_D1_PIN            9
#define SDIO_D1_FUNC           10
#define SDIO_D2_PORT           3 /* PORTD */
#define SDIO_D2_PIN            6
#define SDIO_D2_FUNC           11
#define SDIO_D3_PORT           3 /* PORTD */
#define SDIO_D3_PIN            7
#define SDIO_D3_FUNC           11

#define SDIO_CK_PORT           6 /* GPIOG */
#define SDIO_CK_PIN            12
#define SDIO_CK_FUNC           11

#define SDIO_CMD_PORT           6 /* GPIOG */
#define SDIO_CMD_PIN            2
#define SDIO_CMD_FUNC           11
#endif
static SD_HandleTypeDef hsd;

//static DMA_HandleTypeDef hdma_sdio_rx;
//static DMA_HandleTypeDef hdma_sdio_tx;

static int stm32cube_sdio_hw_init(void) {
	//SD_HandleTypeDef* sdHandle = &hsd;

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
#if 0
	/* SDIO DMA Init */
    /* SDIO_RX Init */
	hdma_sdio_rx.Instance = DMA2_Stream3;
	hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
	hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
	hdma_sdio_rx.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
	hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK) {
		return -1;
    }

	__HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio_rx);

	/* SDIO_TX Init */
	hdma_sdio_tx.Instance = DMA2_Stream6;
	hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
	hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
	hdma_sdio_tx.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
	hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
	if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK) {
		return -1;
	}

	__HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio_tx);
#endif

	return 0;

}

EMBOX_UNIT_INIT(stm32cube_sdio_init);

static int stm32cube_sdio_init(void) {
	/* It seems that  we can't check directly
	 * if any card present, so we just assume
	 * that we have a single card number zero */
	struct mmc_host *mmc = mmc_alloc_host();
	mmc->ops = &stm32cube_sdio_ops;


	stm32cube_sdio_hw_init();
	/* USER CODE END SDIO_Init 1 */
	hsd.Instance = SDIO_INSTANCE;
	hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;//SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;// SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;// SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDMMC_BUS_WIDE_1B;//SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;//SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = TRANSFER_CLK_DIV;
	/* USER CODE BEGIN SDIO_Init 2 */
	
	// Общая настройка.
	// https://imax9.narod.ru/publs/F407les04.html
  
	// Обход бага с 4-бита инитом.
	// https://community.st.com/t5/stm32cubemx-mcus/sdio-interface-not-working-in-4bits-with-stm32f4-firmware/td-p/591776
  
	if (HAL_SD_Init(&hsd) != HAL_OK)
	{
    	return -1;
	}

	if (HAL_SD_ConfigWideBusOperation(&hsd, SDMMC_BUS_WIDE_4B /* SDIO_BUS_WIDE_4B*/) != HAL_OK) {
		return -1;
	}


	mmc_scan(mmc);

	return 0;
}



static irq_return_t stm32cube_sdio_irq_handler(unsigned int irq_num, void *arg) {
	(void)irq_num;
	(void)arg;
	return 0;
}

STATIC_IRQ_ATTACH(SDIO_IRQ_NUM, stm32cube_sdio_irq_handler, NULL);

#endif