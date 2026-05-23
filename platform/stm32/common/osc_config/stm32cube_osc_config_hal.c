/**
 * @file
 *
 * @date 31.03.2017
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <bsp/stm32cube_hal.h>

int RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct) {
  return HAL_RCC_OscConfig(RCC_OscInitStruct);
}
