/**
  ******************************************************************************
  * @file    stm32f4x7_eth_conf_template.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Configuration file for the STM32F4x7xx Ethernet driver.
  *          This file should be copied to the application folder and renamed to
  *          stm32f4x7_eth_conf.h
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_CONF_H
#define __STM32F4x7_ETH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Uncomment the line below when using time stamping and/or IPv4 checksum offload */
#define USE_ENHANCED_DMA_DESCRIPTORS

/* Uncomment the line below if you want to use user defined Delay function
   (for precise timing), otherwise default _eth_delay_ function defined within
   the Ethernet driver is used (less precise timing) */
/* #define USE_Delay */

#ifdef USE_Delay
  #include "main.h"                /* Header file where the Delay function prototype is exported */
  #define _eth_delay_    Delay     /* User can provide more timing precise _eth_delay_ function
                                      ex. use Systick with time base of 10 ms (as done in the provided
                                      STM32F4x7xx demonstrations) */
#else
  #define _eth_delay_    ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif

/* Uncomment some lines below to make custom configuration of the Ethernet driver buffers */
/* Redefinition of the Ethernet driver buffers size and count */
//#define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for receive */
//#define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for transmit */
//#define ETH_RXBUFNB        20                  /* 20 Rx buffers of size ETH_RX_BUF_SIZE */
#define ETH_TXBUFNB        0


/* PHY configuration section **************************************************/
#ifdef USE_Delay
/* PHY Reset delay */
#define PHY_RESET_DELAY    ((uint32_t)0x000000FF)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY   ((uint32_t)0x00000FFF)
/* Delay when writing to Ethernet registers*/
#define ETH_REG_WRITE_DELAY ((uint32_t)0x00000001)
#else
/* PHY Reset delay */
#define PHY_RESET_DELAY    ((uint32_t)0x000FFFFF)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY   ((uint32_t)0x00FFFFFF)
/* Delay when writing to Ethernet registers*/
#define ETH_REG_WRITE_DELAY ((uint32_t)0x0000FFFF)
#endif

/*******************  PHY Extended Registers section : ************************/

/* The PHY status register value change from a PHY to another, so the user have
   to update this value depending on the used external PHY */
#define PHY_SR    ((uint16_t)31) /* Value for DP83848 PHY */

/* The Speed and Duplex mask values change from a PHY to another, so the user
   have to update this value depending on the used external PHY */
#define PHY_DUPLEX_SPEED_STATUS_MASK  ((uint16_t)0x001C)
#define PHY_100BTX_FULL               (18)
#define PHY_100BTX_HALF								(8)
#define PHY_10M_FULL									(14)
#define PHY_10M_HALF									(4)

   /* Note : Common PHY registers are defined in stm32f4x7_eth.h file */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4x7_ETH_CONF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
