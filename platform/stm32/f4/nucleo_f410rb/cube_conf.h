/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.03.2026
 */

#ifndef STM32_CUBE_CONF_H
#define STM32_CUBE_CONF_H


#define  PREFETCH_ENABLE              1U

/* ################## SPI peripheral configuration ########################## */

/* CRC FEATURE: Use to activate CRC feature inside HAL SPI Driver
* Activated: CRC code is present inside driver
* Deactivated: CRC code cleaned from driver
*/

#define USE_SPI_CRC                     1U

#endif /* STM32_CUBE_CONF_H */
