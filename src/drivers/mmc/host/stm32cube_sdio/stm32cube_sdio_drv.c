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

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>

#include <drivers/gpio.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include "stm32cube_definitions.h"


#include <framework/mod/options.h>


#define USE_DMA               OPTION_GET(NUMBER,use_dma)

#if USE_DMA

#define DMA_TRANSFER_STATE_IDLE      (0)
#define DMA_TRANSFER_STATE_RX_START  (2)
#define DMA_TRANSFER_STATE_RX_FIN    (3)
#define DMA_TRANSFER_STATE_TX_START  (4)
#define DMA_TRANSFER_STATE_TX_FIN    (5)

static volatile int dma_transfer_state = DMA_TRANSFER_STATE_IDLE;
static volatile struct schedee *dma_transfer_thread = NULL;

irq_return_t stm32cube_sdio_dma_rx_irq(unsigned int irq_num, void *dev) {
	SD_HandleTypeDef *phsd;

	phsd = dev;

	HAL_DMA_IRQHandler(phsd->hdmarx);

	if (dma_transfer_state == DMA_TRANSFER_STATE_RX_START) {
		dma_transfer_state = DMA_TRANSFER_STATE_RX_FIN;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}

irq_return_t stm32cube_sdio_dma_tx_irq(unsigned int irq_num, void *dev) {
	SD_HandleTypeDef *phsd;

	phsd = dev;

	HAL_DMA_IRQHandler(phsd->hdmatx);

	if (dma_transfer_state == DMA_TRANSFER_STATE_TX_START) {
		dma_transfer_state = DMA_TRANSFER_STATE_TX_FIN;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}

irq_return_t stm32cube_sdio_irq_handler(unsigned int irq_num, void *arg) {
	(void)irq_num;
	(void)arg;
	HAL_SD_IRQHandler(arg);
	return 0;
}
#endif /* USE_DMA */

#define SDMMC_CMDTIMEOUT                       5000U         /* Command send and response timeout */

static int stm32cube_sdio_send_cmd(struct mmc_host *host, struct mmc_request *req) {
	CUBE_CmdInitTypeDef  sdmmc_cmdinit = {0};
	int response;
	SD_HandleTypeDef *phsd;


	phsd = host->priv;

	if (req->cmd.flags & MMC_RSP_PRESENT) {
		if (req->cmd.flags & MMC_RSP_136) {
			response = CUBE_RESPONSE_LONG;
		}
		else {
			response = CUBE_RESPONSE_SHORT;
		}
	} else {
		response = CUBE_RESPONSE_NO;
	}
  
	/* Set Block Size for Card */ 
	sdmmc_cmdinit.Argument         = (uint32_t)req->cmd.arg;
	sdmmc_cmdinit.CmdIndex         = req->cmd.opcode;
	sdmmc_cmdinit.Response         = response;
	sdmmc_cmdinit.WaitForInterrupt = CUBE_WAIT_NO;
	sdmmc_cmdinit.CPSM             = CUBE_CPSM_ENABLE;
	(void)CUBE_SendCommand(phsd->Instance, &sdmmc_cmdinit);

	log_debug("send cmd (%d) arg (%x) resp (%d) longresp (%d)", req->cmd.opcode, req->cmd.arg,
		 req->cmd.flags & MMC_RSP_PRESENT, req->cmd.flags & MMC_RSP_136);

	return 0;
}

static void stm32cube_sdio_request(struct mmc_host *host, struct mmc_request *req) {
	uint32_t *buff;
	SD_HandleTypeDef *phsd;
	int response_flag = 1;

	assert(host);
	assert(req);

	phsd = host->priv;

	if (req->cmd.flags & MMC_DATA_XFER) {
		int xfer = req->data.blksz * req->data.blocks;

		buff = (void *) req->data.addr;
		log_debug("buff=%p", buff);

		while (xfer > 0) {
			if (req->cmd.flags & MMC_DATA_READ) {
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
#if USE_DMA
				HAL_SD_ReadBlocks_DMA(phsd, (void *) buff, req->cmd.arg, 1);
#else
				HAL_SD_ReadBlocks(phsd, (void *) buff, req->cmd.arg, 1, 10000);
#endif /* USE_DMA */
			} else {
#if USE_DMA
				HAL_SD_Writelocks_DMA(phsd, (void *) req->data.addr, req->cmd.arg, 1);
#else
				HAL_SD_WriteBlocks(phsd, (void *) buff, req->cmd.arg, 1, 10000);
#endif /* USE_DMA */
			}
			xfer -= req->data.blksz;
			buff += req->data.blksz / sizeof (*buff);
		}

		return ;

	}

	stm32cube_sdio_send_cmd(host, req);

	switch (req->cmd.flags & MMC_RSP_MASK) {
		case 0:
			response_flag = 0;
			break;
		case MMC_RSP_R1:
		/* case MMC_RSP_R6: */
		/* case MMC_RSP_R7: */
			SDMMC_GetCmdResp1(phsd->Instance, req->cmd.opcode, SDMMC_CMDTIMEOUT);
			response_flag = 1;
		break;
		case MMC_RSP_R2:
			response_flag = 1;
			SDMMC_GetCmdResp2(phsd->Instance);
		break;
		case MMC_RSP_R3:
			response_flag = 1;
			SDMMC_GetCmdResp3(phsd->Instance);
		break;
		default:
		break;
	}

	if (response_flag) {
		req->cmd.resp[0] = CUBE_GetResponse(phsd->Instance, CUBE_RESP1);
		req->cmd.resp[1] = CUBE_GetResponse(phsd->Instance, CUBE_RESP2);
		req->cmd.resp[2] = CUBE_GetResponse(phsd->Instance, CUBE_RESP3);
		req->cmd.resp[3] = CUBE_GetResponse(phsd->Instance, CUBE_RESP4);
	}

	if (req->cmd.flags & MMC_DATA_XFER) {
		int xfer = req->data.blksz * req->data.blocks;

		buff = (void *) req->data.addr;
		log_debug("buff=%p", buff);

		while (xfer > 0) {
			if (req->cmd.flags & MMC_DATA_READ) {
				#if 0
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
				#endif
			} else {
				log_debug("writting  %d byte", req->data.blksz);
			}
		}
	}
}

const struct mmc_host_ops stm32cube_sdio_ops = {
	.request = stm32cube_sdio_request,
};
