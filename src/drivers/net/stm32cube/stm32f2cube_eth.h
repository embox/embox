/**
 * @file
 *
 * @date Aug 24, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_STM32CUBE_STM32F2CUBE_ETH_H_
#define SRC_DRIVERS_NET_STM32CUBE_STM32F2CUBE_ETH_H_


//#include <bsp/stm32cube_hal.h>

//#define PHY_ADDRESS       0x00U
//#define PHY_ADDRESS       LAN8742A_PHY_ADDRESS

#include <config/board_config.h>

#define PHY_ADDRESS   CONF_ETH_MISC_PHY_ADDR

#endif /* SRC_DRIVERS_NET_STM32CUBE_STM32F2CUBE_ETH_H_ */
