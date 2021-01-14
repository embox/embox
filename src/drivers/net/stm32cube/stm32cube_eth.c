/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.08.2014
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <util/math.h>
#include <kernel/irq.h>
#include <hal/reg.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/util/show_packet.h>
#include <drivers/net/stm32cube_eth.h>
#include <util/log.h>
#include <embox/unit.h>
#include <arm/cpu_cache.h>

EMBOX_UNIT_INIT(stm32eth_init);

#define STM32ETH_IRQ   OPTION_GET(NUMBER, irq)

static ETH_HandleTypeDef stm32_eth_handler;
#ifdef STM32H7_CUBE
static ETH_TxPacketConfig TxConfig;
#endif

static ETH_DMADescTypeDef DMARxDscrTab[ETH_RXBUFNB] \
	__attribute__ ((aligned (4))) SRAM_DEVICE_MEM_SECTION;

static uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] \
	 __attribute__ ((aligned (4))) SRAM_NOCACHE_SECTION;

#if ETH_TXBUFNB == 0
#undef  ETH_TXBUFNB
#define ETH_TXBUFNB 4
#define TX_NO_BUFF  1
#endif

#ifdef TX_NO_BUFF
#ifndef STM32H7_CUBE
static uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] \
	__attribute__ ((aligned (4))) SRAM_NOCACHE_SECTION;
#endif /* STM32H7_CUBE */
static ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB] \
	__attribute__ ((aligned (4))) SRAM_DEVICE_MEM_SECTION;
#else /* TX_NO_BUFF */
static uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] \
	__attribute__ ((aligned (4))) SRAM_NOCACHE_SECTION;

static ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB] \
	__attribute__ ((aligned (4))) SRAM_DEVICE_MEM_SECTION;
#endif /* TX_NO_BUFF */

static void low_level_init(unsigned char mac[6]) {
	memset(&stm32_eth_handler, 0, sizeof(stm32_eth_handler));

	stm32_eth_handler.Instance = (ETH_TypeDef *) ETH_BASE;
	/* Fill ETH_InitStructure parametrs */
	stm32_eth_handler.Init.MACAddr = mac;
#ifdef STM32H7_CUBE
	stm32_eth_handler.Init.MediaInterface = HAL_ETH_RMII_MODE;
	stm32_eth_handler.Init.RxDesc = DMARxDscrTab;
	stm32_eth_handler.Init.TxDesc = DMATxDscrTab;
	stm32_eth_handler.Init.RxBuffLen = ETH_TX_BUF_SIZE;
#else
	stm32_eth_handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_DISABLE;
	stm32_eth_handler.Init.Speed = ETH_SPEED_100M;
	stm32_eth_handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
	stm32_eth_handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
	stm32_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_SOFTWARE;
	stm32_eth_handler.Init.PhyAddress = PHY_ADDRESS;
	stm32_eth_handler.Init.RxMode = ETH_RXINTERRUPT_MODE;
#endif

	if (HAL_OK != HAL_ETH_Init(&stm32_eth_handler)) {
		log_error("HAL_ETH_Init error\n");
	}
#ifdef STM32H7_CUBE
	if (stm32_eth_handler.gState == HAL_ETH_STATE_READY) {

	}
	for (int idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
	{
		HAL_ETH_DescAssignMemory(&stm32_eth_handler, idx, Rx_Buff[idx], NULL);
	}

	/* Set Tx packet config common parameters */
	memset(&TxConfig, 0, sizeof(ETH_TxPacketConfig));
	TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
	TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

	ETH_MACConfigTypeDef MACConf;
	/* Get MAC Config MAC */
	HAL_ETH_GetMACConfig(&stm32_eth_handler, &MACConf);
	MACConf.DuplexMode = ETH_FULLDUPLEX_MODE;
	MACConf.Speed = ETH_SPEED_100M;
	HAL_ETH_SetMACConfig(&stm32_eth_handler, &MACConf);

	/* (#)Enable MAC and DMA transmission and reception: */
	HAL_ETH_Start_IT(&stm32_eth_handler);
#else

	if (stm32_eth_handler.State == HAL_ETH_STATE_READY) {
		log_info("STATE_READY sp %d duplex %d\n",
			stm32_eth_handler.Init.Speed, stm32_eth_handler.Init.DuplexMode);
	}

	/*(#)Initialize Ethernet DMA Descriptors in chain mode and point to allocated buffers:*/
	HAL_ETH_DMATxDescListInit(&stm32_eth_handler, DMATxDscrTab, &Tx_Buff[0][0],
			ETH_TXBUFNB); /*for Transmission process*/
	if (HAL_OK != HAL_ETH_DMARxDescListInit(&stm32_eth_handler,
			DMARxDscrTab, &Rx_Buff[0][0],
			ETH_RXBUFNB)) { /*for Reception process*/
		log_error("HAL_ETH_DMARxDescListInit error\n");
	}

	/* (#)Enable MAC and DMA transmission and reception: */
	HAL_ETH_Start(&stm32_eth_handler);
#endif
}

static struct sk_buff *low_level_input(void) {
	struct sk_buff *skb;
	int len;
	uint8_t *buffer;

#ifdef STM32H7_CUBE
	ETH_BufferTypeDef RxBuff;
	uint32_t framelength = 0;
	if (!HAL_ETH_IsRxDataAvailable(&stm32_eth_handler)) {
		return NULL;
	}
	HAL_ETH_GetRxDataBuffer(&stm32_eth_handler, &RxBuff);
	HAL_ETH_GetRxDataLength(&stm32_eth_handler, &framelength);

	/* Build Rx descriptor to be ready for next data reception */
	HAL_ETH_BuildRxDescriptors(&stm32_eth_handler);

	/* Invalidate data cache for ETH Rx Buffers */
	//SCB_InvalidateDCache_by_Addr((uint32_t *)RxBuff.buffer, framelength);

	buffer = RxBuff.buffer;
	len = framelength;


#else
	/* get received frame */
	if (HAL_ETH_GetReceivedFrame_IT(&stm32_eth_handler) != HAL_OK) {
		return NULL;
	}

	/* Obtain the size of the packet and put it into the "len" variable. */
	len = stm32_eth_handler.RxFrameInfos.length;
	buffer = (uint8_t *) stm32_eth_handler.RxFrameInfos.buffer;
#endif
	skb = NULL;

	/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
	skb = skb_alloc(len);

	/* copy received frame to pbuf chain */
	if (skb != NULL) {
		memcpy(skb->mac.raw, buffer, len);
	} else {
		log_error("skb_alloc failed\n");
	}
#ifndef STM32H7_CUBE
	{
		uint32_t i = 0;
		__IO ETH_DMADescTypeDef *dmarxdesc;

		/* Release descriptors to DMA */
		dmarxdesc = stm32_eth_handler.RxFrameInfos.FSRxDesc;

		/* Set Own bit in Rx descriptors: gives the buffers back to DMA */
		for (i = 0; i < stm32_eth_handler.RxFrameInfos.SegCount; i++) {
			dmarxdesc->Status |= ETH_DMARXDESC_OWN;
			dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
		}

		/* Clear Segment_Count */
		stm32_eth_handler.RxFrameInfos.SegCount = 0;

		/* When Rx Buffer unavailable flag is set: clear it and resume reception */
		if ((stm32_eth_handler.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
			/* Clear RBUS ETHERNET DMA flag */
			stm32_eth_handler.Instance->DMASR = ETH_DMASR_RBUS;
			/* Resume DMA reception */
			stm32_eth_handler.Instance->DMARPDR = 0;
		}
	}
#endif
	return skb;
}


static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb);
static int stm32eth_open(struct net_device *dev);
static int stm32eth_set_mac(struct net_device *dev, const void *addr);
static const struct net_driver stm32eth_ops = {
		.xmit = stm32eth_xmit,
		.start = stm32eth_open,
		.set_macaddr = stm32eth_set_mac,
};

static int stm32eth_open(struct net_device *dev) {
	low_level_init(dev->dev_addr);

	return 0;
}

static int stm32eth_set_mac(struct net_device *dev, const void *addr) {
	ETH_TypeDef *regs = (ETH_TypeDef *) ETH_BASE;

	memcpy(dev->dev_addr, addr, ETH_ALEN);

	regs->MACA0HR = ((uint32_t) dev->dev_addr[5] << 8) |
	                ((uint32_t) dev->dev_addr[4] << 0);

	regs->MACA0LR = ((uint32_t) dev->dev_addr[3] << 24) |
	                ((uint32_t) dev->dev_addr[2] << 16) |
	                ((uint32_t) dev->dev_addr[1] << 8)  |
	                ((uint32_t) dev->dev_addr[0] << 0);

	return ENOERR;
}

#ifdef TX_NO_BUFF
static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
#ifdef STM32H7_CUBE
#define ETH_DMA_TRANSMIT_TIMEOUT                (20U)
	uint32_t  framelen = 0;
	ETH_BufferTypeDef Txbuffer;

	memset(&Txbuffer, 0, sizeof(Txbuffer));
	Txbuffer.buffer = skb->mac.raw;
	Txbuffer.len = skb->len;

	TxConfig.Length = framelen;
	TxConfig.TxBuffer = &Txbuffer;
	HAL_ETH_Transmit(&stm32_eth_handler, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

#else
	__IO ETH_DMADescTypeDef *dma_tx_desc;

	dma_tx_desc = stm32_eth_handler.TxDesc;
	/* Transmit skbuff directly without any buffer. */
	dma_tx_desc->Buffer1Addr = (uint32_t) skb->mac.raw;

	/* Prepare transmit descriptors to give to DMA */
	if (0 != HAL_ETH_TransmitFrame(&stm32_eth_handler, skb->len)) {
		log_error("HAL_ETH_TransmitFrame failed\n");
		return -1;
	}
	/* Wait until paacket transmitted and descriptor released. */
	while (dma_tx_desc->Status & ETH_DMATXDESC_OWN)
		;
#endif
	skb_free(skb);

	return 0;
}
#else
static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
	__IO ETH_DMADescTypeDef *dma_tx_desc;

	dma_tx_desc = stm32_eth_handler.TxDesc;
	memcpy((void *)dma_tx_desc->Buffer1Addr, skb->mac.raw, skb->len);

	/* Prepare transmit descriptors to give to DMA */
	if (0 != HAL_ETH_TransmitFrame(&stm32_eth_handler, skb->len)) {
		log_error("HAL_ETH_TransmitFrame failed\n");
		return -1;
	}
	skb_free(skb);

	return 0;
}
#endif

static irq_return_t stm32eth_interrupt(unsigned int irq_num, void *dev_id) {
	struct net_device *nic_p = dev_id;
	struct sk_buff *skb;
	ETH_HandleTypeDef *heth = &stm32_eth_handler;

	if (!nic_p) {
		return IRQ_NONE;
	}
#ifdef STM32H7_CUBE
	/* Packet received */
	if (__HAL_ETH_DMA_GET_IT(heth, ETH_DMACSR_RI))
	{
		if(__HAL_ETH_DMA_GET_IT_SOURCE(heth, ETH_DMACIER_RIE))
		{
			/* Receive complete callback */
			while (NULL != (skb = low_level_input())) {
				skb->dev = nic_p;

				show_packet(skb->mac.raw, skb->len, "rx");

				netif_rx(skb);
			}
			/* Clear the Eth DMA Rx IT pending bits */
			__HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMACSR_RI | ETH_DMACSR_NIS);
		}
	}

#else
	if (heth->Instance->DMASR & ETH_DMA_FLAG_FBE) {
		log_error("DMA error: DMASR = 0x%08x\n", heth->Instance->DMASR);
	}

	/* Frame received */
	if (__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_R)) {
		/* Receive complete callback */
		while (NULL != (skb = low_level_input())) {
			skb->dev = nic_p;

			show_packet(skb->mac.raw, skb->len, "rx");

			netif_rx(skb);
		}
		/* Clear the Eth DMA Rx IT pending bits */
		__HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_R);
	}
	__HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_NIS);
#endif
	return IRQ_HANDLED;
}

static struct net_device *stm32eth_netdev;
static int stm32eth_init(void) {
	int res;
	struct net_device *nic;

	nic = (struct net_device *) etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &stm32eth_ops;
	nic->irq = STM32ETH_IRQ;
	nic->base_addr = ETH_BASE;

	stm32eth_netdev = nic;

	res = irq_attach(nic->irq, stm32eth_interrupt, 0, stm32eth_netdev, "");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}

STATIC_IRQ_ATTACH(STM32ETH_IRQ, stm32eth_interrupt, stm32eth_netdev);
