/**
 * @file
 *
 * @date 17.03.2017
 * @author Anton Bondarev
 */

#ifndef STM32F4CUBE_ETH_H_
#define STM32F4CUBE_ETH_H_

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_eth.h>

#if defined(STM32F407xx)
#define PHY_ADDRESS       DP83848_PHY_ADDRESS /* FIXME Relative to STM324xG-EVAL Board */
#elif defined(STM32F429xx)
#define PHY_ADDRESS       LAN8742A_PHY_ADDRESS
#else
#error "don't suppurt platform"
#endif

#endif /* STM32F4CUBE_ETH_H_ */
