/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.08.2014
 */
#include <util/log.h>

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

#include <embox/unit.h>
#include <arm/cpu_cache.h>

EMBOX_UNIT_INIT(stm32eth_init);

#define ETH_DMA_TRANSMIT_TIMEOUT                (20U)

#define STM32ETH_IRQ   OPTION_GET(NUMBER, irq)
#define USE_RMII       OPTION_GET(NUMBER, use_rmii)

static ETH_HandleTypeDef stm32_eth_handler;
static ETH_TxPacketConfig TxConfig;

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
#if USE_RMII
	stm32_eth_handler.Init.MediaInterface = HAL_ETH_RMII_MODE;
#else
	stm32_eth_handler.Init.MediaInterface = HAL_ETH_MII_MODE;
#endif
	stm32_eth_handler.Init.RxDesc = DMARxDscrTab;
	stm32_eth_handler.Init.TxDesc = DMATxDscrTab;
	stm32_eth_handler.Init.RxBuffLen = ETH_TX_BUF_SIZE;

	if (HAL_OK != HAL_ETH_Init(&stm32_eth_handler)) {
		log_error("HAL_ETH_Init error\n");
	}

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
}

static uint8_t *low_level_input(int *len) {
	uint8_t *buffer;

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
	*len = framelength;
	return buffer;
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
static uint8_t Tx_Buff[ETH_TX_BUF_SIZE] \
	__attribute__ ((aligned (4))) SRAM_NOCACHE_SECTION;
static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
	ETH_BufferTypeDef Txbuffer;

	show_packet(skb->mac.raw, skb->len, "tx");

	memcpy((void *)Tx_Buff, skb->mac.raw, skb->len);

	memset(&Txbuffer, 0, sizeof(Txbuffer));
	Txbuffer.buffer = Tx_Buff;
	Txbuffer.len = skb->len;

	TxConfig.Length = skb->len;
	TxConfig.TxBuffer = &Txbuffer;
	HAL_ETH_Transmit(&stm32_eth_handler, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

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

	/* Packet received */
	if (__HAL_ETH_DMA_GET_IT(heth, ETH_DMACSR_RI))
	{
		if(__HAL_ETH_DMA_GET_IT_SOURCE(heth, ETH_DMACIER_RIE))
		{
			uint8_t *buffer;
			int len;
			/* Receive complete callback */
			while (NULL != (buffer = low_level_input(&len))) {

				/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
				skb = skb_alloc(len);

				if (!skb) {
					log_error("skb_alloc failed\n");
					continue;
				}
				/* copy received frame to pbuf chain */
				memcpy(skb->mac.raw, buffer, len);

				skb->dev = nic_p;

				show_packet(skb->mac.raw, skb->len, "rx");

				netif_rx(skb);
			}
			/* Clear the Eth DMA Rx IT pending bits */
			__HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMACSR_RI | ETH_DMACSR_NIS);
		}
	}

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

