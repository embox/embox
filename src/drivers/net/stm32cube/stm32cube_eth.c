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


#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_eth.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32eth_init);

#define STM32ETH_IRQ (ETH_IRQn + 16)

#define PHY_ADDRESS       0x01 /* Relative to STM324xG-EVAL Board */

static ETH_HandleTypeDef stm32_eth_handler;


void HAL_ETH_MspInit(ETH_HandleTypeDef *heth) {
   /*(##) Enable the Ethernet interface clock using
         (+++) __HAL_RCC_ETHMAC_CLK_ENABLE();
         (+++) __HAL_RCC_ETHMACTX_CLK_ENABLE();
         (+++) __HAL_RCC_ETHMACRX_CLK_ENABLE();

    (##) Initialize the related GPIO clocks
    (##) Configure Ethernet pin-out
    (##) Configure Ethernet NVIC interrupt (IT mode)
    */

	volatile uint32_t i;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	  /* Enable GPIOs clocks */
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
#if 0
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* MII/RMII Media interface selection --------------------------------------*/
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif

	/* Ethernet pins configuration ************************************************/
	/*
	   ETH_MDIO --------------> PA2
	   ETH_MDC ---------------> PC1

	   ETH_RMII_REF_CLK-------> PA1

	   ETH_RMII_CRS_DV -------> PA7
	   ETH_MII_RX_ER   -------> PB10
	   ETH_RMII_RXD0   -------> PC4
	   ETH_RMII_RXD1   -------> PC5
	   ETH_RMII_TX_EN  -------> PB11
	   ETH_RMII_TXD0   -------> PB12
	   ETH_RMII_TXD1   -------> PB13

	   ETH_RST_PIN     -------> PE2
	   */

	/* Configure PA1,PA2 and PA7 */
	GPIO_InitStructure.Pin   = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* Configure PB10,PB11,PB12 and PB13 */
	GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
//	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
	//GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable ETHERNET clock  */
	__HAL_RCC_ETH_CLK_ENABLE();

	if (heth->Init.MediaInterface == ETH_MEDIA_INTERFACE_MII) {
	    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
	    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
	}

	/* Configure the PHY RST  pin */
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
	for (i = 0; i < 20000; i++);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
	for (i = 0; i < 20000; i++);

}

/* Minimum value: 2 */
#define TXTAB_LEN 2



static ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB]__attribute__ ((aligned (4)));
static uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __attribute__ ((aligned (4)));

static ETH_DMADescTypeDef  DMATxDscrTab[TXTAB_LEN]__attribute__ ((aligned (4)));
static uint8_t Tx_Buff[TXTAB_LEN][ETH_TX_BUF_SIZE] __attribute__ ((aligned (4)));


struct stm32eth_state {
	ETH_DMADescTypeDef tx_tab[TXTAB_LEN];
	ETH_DMADescTypeDef *tx_head;
	ETH_DMADescTypeDef *tx_tail;
	struct sk_buff *tx_skb[TXTAB_LEN];
};
static struct stm32eth_state stm32eth_g_state;

static inline int stm32eth_desc2i(ETH_DMADescTypeDef *base, ETH_DMADescTypeDef *desc) {
	return ((intptr_t) desc - (intptr_t) base) / sizeof(*desc);
}

static void stm32eth_tx_skb_set(struct stm32eth_state *state, struct sk_buff *skb) {
	int i_desc = stm32eth_desc2i(state->tx_tab, state->tx_tail);
	assert(state->tx_skb[i_desc] == NULL);
	state->tx_skb[i_desc] = skb;
}

static void low_level_init(unsigned char mac[6]) {
	memset(&stm32_eth_handler, 0, sizeof(stm32_eth_handler));
	stm32_eth_handler.Instance = (ETH_TypeDef *)ETH_BASE;
	/* Fill ETH_InitStructure parametrs */
	stm32_eth_handler.Init.MACAddr = mac;
	stm32_eth_handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
	stm32_eth_handler.Init.Speed = ETH_SPEED_100M;
	stm32_eth_handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
	stm32_eth_handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_MII;
	stm32_eth_handler.Init.RxMode = ETH_RXPOLLING_MODE;
	stm32_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
	stm32_eth_handler.Init.PhyAddress = PHY_ADDRESS;
	stm32_eth_handler.Init.RxMode = ETH_RXINTERRUPT_MODE;
#ifdef CHECKSUM_BY_HARDWARE
	stm32_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
#endif

	HAL_ETH_Init(&stm32_eth_handler);

	/*(#)Initialize Ethernet DMA Descriptors in chain mode and point to allocated buffers:*/
    HAL_ETH_DMATxDescListInit(&stm32_eth_handler, DMATxDscrTab, &Tx_Buff[0][0], TXTAB_LEN); /*for Transmission process*/
    HAL_ETH_DMARxDescListInit(&stm32_eth_handler, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB); /*for Reception process*/

   /* (#)Enable MAC and DMA transmission and reception: */
	HAL_ETH_Start(&stm32_eth_handler);
}
#if 0
static struct sk_buff *low_level_input(void) {
	struct sk_buff *skb;
	int len;
	FrameTypeDef frame;
	u8 *buffer;
	uint32_t i=0;
	__IO ETH_DMADESCTypeDef *DMARxNextDesc;


	skb = NULL;

	/* get received frame */
	frame = ETH_Get_Received_Frame();

	/* Obtain the size of the packet and put it into the "len" variable. */
	len = frame.length;
	buffer = (u8 *)frame.buffer;

	/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
	skb = skb_alloc(len);

	/* copy received frame to pbuf chain */
	if (skb != NULL) {
		memcpy(skb->mac.raw, buffer, len);
	}

	/* Release descriptors to DMA */
	/* Check if frame with multiple DMA buffer segments */
	if (DMA_RX_FRAME_infos->Seg_Count > 1) {
		DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
	} else {
		DMARxNextDesc = frame.descriptor;
	}

	/* Set Own bit in Rx descriptors: gives the buffers back to DMA */
	for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++) {
		DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
		DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
	}

	/* Clear Segment_Count */
	DMA_RX_FRAME_infos->Seg_Count =0;

	/* When Rx Buffer unavailable flag is set: clear it and resume reception */
	if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET) {
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_RBUS;
		/* Resume DMA reception */
		ETH->DMARPDR = 0;
	}
	return skb;
}
#endif
/*********** COPYRIGHT 2004 Swedish Institute of Computer Science *****END OF FILE****/

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
	//ETH_HandleTypeDef *heth = (ETH_HandleTypeDef *)ETH_BASE;

	memcpy(dev->dev_addr, addr, ETH_ALEN);

	//ETH_MACAddressConfig(heth, ETH_MAC_ADDRESS0, dev->dev_addr);

	return ENOERR;
}

static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct stm32eth_state *state = &stm32eth_g_state;
	ETH_DMADescTypeDef *next_tail = (ETH_DMADescTypeDef *) state->tx_tail->Buffer2NextDescAddr;

	if (next_tail == state->tx_head) {
		return -EBUSY;
	}

	state->tx_tail->Buffer1Addr = (uint32_t) skb->mac.raw;
	state->tx_tail->ControlBufferSize = skb->len & ETH_DMATXDESC_TBS1;
	state->tx_tail->Status = ETH_DMATXDESC_FS | ETH_DMATXDESC_LS | ETH_DMATXDESC_TCH
		 | ETH_DMATXDESC_IC | ETH_DMATXDESC_OWN;
	stm32eth_tx_skb_set(state, skb);

	state->tx_tail = next_tail;

	REG_STORE(&ETH->DMATPDR, 0);
	return 0;
}

static irq_return_t stm32eth_interrupt(unsigned int irq_num, void *dev_id) {
	//struct net_device *nic_p = dev_id;

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
	/*nic_priv = netdev_priv(nic, struct stm32eth_priv);*/

	stm32eth_netdev = nic;

	res = irq_attach(nic->irq, stm32eth_interrupt, 0, stm32eth_netdev, "");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}

/* STM32ETH_IRQ defined through ETH_IRQn, which as enum and
 * can't expand into int, as STATIC_IRQ_ATTACH require
 */
static_assert(77 == STM32ETH_IRQ);
STATIC_IRQ_ATTACH(77, stm32eth_interrupt, stm32eth_netdev);
