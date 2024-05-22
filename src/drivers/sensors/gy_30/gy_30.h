/**
 * @file
 *
 * @date 29.03.2017
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_
#define PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_

/* TODO Support other modes */
#define BH1750_CONTINUOUS_HIGH_RES_MODE  0x10
#define BH1750_ONE_TIME_HIGH_RES_MODE    0x20

/* Here ID corresponds to I2C bus number to which Gy-30 is attached */
extern void gy_30_setup_mode(unsigned char id, uint8_t mode);
extern uint16_t gy_30_read_light_level(unsigned char id);

#endif /* PLATFORM_STM32F4_MULTIBOTS_CMDS_GY_30_H_ */
