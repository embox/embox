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
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <hal/reg.h>
#include <stm32f4xx.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4x7_eth.h>
#include <misc.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32eth_init);

#define STM32ETH_IRQ (ETH_IRQn + 16)

/**
 ******************************************************************************
 * @file    stm32f4x7_eth_bsp.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    31-October-2011
 * @brief   STM32F4x7 Ethernet hardware configuration.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */
/**
 ******************************************************************************
 * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
 * @file    stm32f4x7_eth_bsp.c
 * @author  CMP Team
 * @version V1.0.0
 * @date    28-December-2012
 * @brief   STM32F4x7 Ethernet hardware configuration.
 *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB and
 *          STM32F4DIS-LCD modules.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 ******************************************************************************
 */

#define LAN8720_PHY_ADDRESS       0x01 /* Relative to STM324xG-EVAL Board */

static void ETH_GPIO_Config(void);
static uint32_t ETH_MACDMA_Config(void);
static uint32_t ETH_BSP_Config(void) {

	/* Configure the GPIO ports for ethernet pins */
	ETH_GPIO_Config();

	/* Configure the Ethernet MAC/DMA */
	return ETH_MACDMA_Config();
}

static uint32_t ETH_MACDMA_Config(void) {
	ETH_InitTypeDef ETH_InitStructure;

	/* Enable ETHERNET clock  */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
			RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

	/* Reset ETHERNET on AHB Bus */
	ETH_DeInit();

	/* Software reset */
	ETH_SoftwareReset();

	/* Wait for software reset */
	while (ETH_GetSoftwareResetStatus() == SET);

	/* ETHERNET Configuration --------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);

	/* Fill ETH_InitStructure parametrs */
	/*------------------------   MAC   -----------------------------------*/
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	//ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
	//  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
	//  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;

	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

	/*------------------------   DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	   the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
	   if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	/* Configure Ethernet */
	return ETH_Init(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
}

static void ETH_GPIO_Config(void) {
	volatile uint32_t i;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB
			| RCC_AHB1Periph_GPIOC, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* MII/RMII Media interface selection --------------------------------------*/
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);


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
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	/* Configure PB10,PB11,PB12 and PB13 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);

	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

	/* Configure the PHY RST  pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	for (i = 0; i < 20000; i++);
	GPIO_SetBits(GPIOE, GPIO_Pin_2);
	for (i = 0; i < 20000; i++);
}

/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* Minimum value: 2 */
#define TXTAB_LEN 2

typedef ETH_DMADESCTypeDef eth_dma_desc_t;

struct stm32eth_state {
	eth_dma_desc_t tx_tab[TXTAB_LEN];
	eth_dma_desc_t *tx_head;
	eth_dma_desc_t *tx_tail;
	struct sk_buff *tx_skb[TXTAB_LEN];
};
static struct stm32eth_state stm32eth_g_state;

static inline int stm32eth_desc2i(eth_dma_desc_t *base, eth_dma_desc_t *desc) {
	return ((intptr_t) desc - (intptr_t) base) / sizeof(*desc); 
}

static void stm32eth_tx_skb_set(struct stm32eth_state *state, struct sk_buff *skb) {
	int i_desc = stm32eth_desc2i(state->tx_tab, state->tx_tail);
	assert(state->tx_skb[i_desc] == NULL);
	state->tx_skb[i_desc] = skb;
}

static struct sk_buff *stm32eth_tx_skb_get(struct stm32eth_state *state) {
	int i_desc = stm32eth_desc2i(state->tx_tab, state->tx_head);
	struct sk_buff *skb = state->tx_skb[i_desc];
	assert(skb != NULL);
	state->tx_skb[i_desc] = NULL;
	return skb;
}

extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

static void stm32eth_dma_tx_init(struct stm32eth_state *state) {

	state->tx_head = state->tx_tail = state->tx_tab;

	for (int i = 0; i < TXTAB_LEN - 1; ++i) {
		state->tx_tab[i].Buffer2NextDescAddr = (uint32_t) &state->tx_tab[i + 1];
	}
	state->tx_tab[TXTAB_LEN - 1].Buffer2NextDescAddr = (uint32_t) state->tx_tab;

	REG_STORE(&ETH->DMATDLAR, (uint32_t) state->tx_tab);
}

static void low_level_init(unsigned char mac[6]) {
	ETH_MACAddressConfig(ETH_MAC_Address0, mac);

	stm32eth_dma_tx_init(&stm32eth_g_state);

	/* Initialize Rx Descriptors list: Chain Mode  */
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

	/* Clear pending flags */
	ETH_DMAClearFlag(~0xFFFE1800);
	/* Enable iterrupts */
	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);

	/* Enable MAC and DMA transmission and reception */
	ETH_Start();
}

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
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	ETH_MACAddressConfig(ETH_MAC_Address0, dev->dev_addr);
	return ENOERR;
}

static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct stm32eth_state *state = &stm32eth_g_state;
	eth_dma_desc_t *next_tail = (eth_dma_desc_t *) state->tx_tail->Buffer2NextDescAddr;

	if (next_tail == state->tx_head) {
		return -EBUSY;
	}

	state->tx_tail->Buffer1Addr = (uint32_t) skb->mac.raw;
	state->tx_tail->ControlBufferSize = skb->len & ETH_DMATxDesc_TBS1;
	state->tx_tail->Status = ETH_DMATxDesc_FS | ETH_DMATxDesc_LS | ETH_DMATxDesc_TCH
		 | ETH_DMATxDesc_IC | ETH_DMATxDesc_OWN;
	stm32eth_tx_skb_set(state, skb);

	state->tx_tail = next_tail;

	REG_STORE(&ETH->DMATPDR, 0);
	return 0;
}

static void stm32eth_txed_collect(struct stm32eth_state *state) {
	while (state->tx_head != state->tx_tail) {
		if (state->tx_head->Status & ETH_DMATxDesc_OWN) {
			break;
		}
		skb_free(stm32eth_tx_skb_get(state));
		state->tx_head = (eth_dma_desc_t *) state->tx_head->Buffer2NextDescAddr;
	}
}

static irq_return_t stm32eth_interrupt(unsigned int irq_num, void *dev_id) {
	struct net_device **nic_p = dev_id;
	if (!*nic_p) {
		return IRQ_NONE;
	}

	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T);

	stm32eth_txed_collect(&stm32eth_g_state);

	while(ETH_CheckFrameReceived()) {
		struct sk_buff *skb = low_level_input();
		if (skb) {
			skb->dev = *nic_p;
			netif_rx(skb);
		}
	}

	return IRQ_HANDLED;
}

static struct net_device *stm32eth_netdev;
static int stm32eth_init(void) {
	int res;
	struct net_device *nic;

	if (0 == ETH_BSP_Config()) {
		return -EIO;
	}

	nic = (struct net_device *) etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &stm32eth_ops;
	nic->irq = STM32ETH_IRQ;
	nic->base_addr = ETH_BASE;
	/*nic_priv = netdev_priv(nic, struct stm32eth_priv);*/

	stm32eth_netdev = nic;

	res = irq_attach(nic->irq, stm32eth_interrupt, 0, &stm32eth_netdev, "");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}

/* STM32ETH_IRQ defined through ETH_IRQn, which as enum and
 * can't expand into int, as STATIC_IRQ_ATTACH require
 */
static_assert(77 == STM32ETH_IRQ);
STATIC_IRQ_ATTACH(77, stm32eth_interrupt, &stm32eth_netdev);
