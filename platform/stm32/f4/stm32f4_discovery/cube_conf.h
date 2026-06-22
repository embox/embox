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

#if defined(STM32F4_CUBE_1_25_0)
#define HAL_ETH_MODULE_ENABLED
#else
/* since 1.27.1 version use ETH_LEGACY version*/
/* #define HAL_ETH_MODULE_ENABLED */
#define HAL_ETH_LEGACY_MODULE_ENABLED
#endif /* STM32F4_CUBE_1_25_0 */


/* Section 2: PHY configuration section */

/* PHY Reset delay these values are based on a 1 ms Systick interrupt*/
#define PHY_RESET_DELAY                 (0x00000020U)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY                (0x00000020U)

#define PHY_READ_TO                     (0x0000FFFFU)
#define PHY_WRITE_TO                    (0x0000FFFFU)

#include <stm32/stm32cube_conf_eth_phy.h>

/* ################## SPI peripheral configuration ########################## */

/* CRC FEATURE: Use to activate CRC feature inside HAL SPI Driver
* Activated: CRC code is present inside driver
* Deactivated: CRC code cleaned from driver
*/

#define USE_SPI_CRC                     1U

#endif /* STM32_CUBE_CONF_H */
