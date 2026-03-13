/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.03.2026
 */

#ifndef STM32_CUBE_CONF_H
#define STM32_CUBE_CONF_H

/* The prefetch will be enabled in SystemClock_Config(), depending on the used 
  STM32F405/415/07/417 device: RevA (prefetch must be off) or RevZ (prefetch can be on/off) */
#define  PREFETCH_ENABLE              0U 

/* ################## SPI peripheral configuration ########################## */

/* CRC FEATURE: Use to activate CRC feature inside HAL SPI Driver
* Activated: CRC code is present inside driver
* Deactivated: CRC code cleaned from driver
*/

#define USE_SPI_CRC                     1U

#endif /* STM32_CUBE_CONF_H */
