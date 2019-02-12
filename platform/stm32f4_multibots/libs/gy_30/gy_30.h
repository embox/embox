/**
 * @file
 *
 * @data 29.03.2017
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_
#define PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

/* TODO Support other modes */
#define BH1750_CONTINUOUS_HIGH_RES_MODE  0x10
#define BH1750_ONE_TIME_HIGH_RES_MODE    0x20

extern int gy_30_init(int i2c_nr);
extern void gy_30_setup_mode(uint8_t mode);
extern uint16_t gy_30_read_light_level(void);

#endif /* PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_ */
