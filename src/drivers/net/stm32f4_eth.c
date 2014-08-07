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
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4x7_eth.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32eth_init);

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


static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb);
static int stm32eth_open(struct net_device *dev);
static int stm32eth_set_mac(struct net_device *dev, const void *addr);
static const struct net_driver stm32eth_ops = {
	.xmit = stm32eth_xmit,
	.start = stm32eth_open,
	.set_macaddr = stm32eth_set_mac,
};

static int stm32eth_open(struct net_device *dev) {
	return 0;
}

static int stm32eth_set_mac(struct net_device *dev, const void *addr) {
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	return ENOERR;
}

static int stm32eth_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

#if 0
static int stm32eth_setup(struct net_device *dev) {
	dev->mtu      = (16 * 1024) + 20 + 20 + 12;
	dev->hdr_len  = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->type     = ARP_HRD_LOOPBACK;
	dev->flags    = IFF_NOARP | IFF_RUNNING;
	dev->drv_ops  = &stm32eth_ops;
	dev->ops      = &ethernet_ops;
	return 0;
}
#endif

static irq_return_t stm32eth_interrupt(unsigned int irq_num, void *dev_id) {
	return IRQ_HANDLED;
}

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
	nic->irq = 73;
	/*nic->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;*/
	/*nic_priv = netdev_priv(nic, struct stm32eth_priv);*/

	res = irq_attach(nic->irq, stm32eth_interrupt, 0, nic, "");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);

}
