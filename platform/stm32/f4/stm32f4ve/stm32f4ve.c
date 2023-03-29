#include "stm32f4ve.h"

#define __STM32F4VE_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F4VE_BSP_VERSION_SUB1   (0x01) /*!< [23:16] sub1 version */
#define __STM32F4VE_BSP_VERSION_SUB2   (0x01) /*!< [15:8]  sub2 version */
#define __STM32F4VE_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __STM32F4VE_BSP_VERSION         ((__STM32F4VE_BSP_VERSION_MAIN << 24)\
                                             |(__STM32F4VE_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32F4VE_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32F4VE_BSP_VERSION_RC)) 
uint32_t BSP_GetVersion(void)
{
  return __STM32F4VE_BSP_VERSION;
}

