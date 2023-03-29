#ifndef __STM32F4VE_H
#define __STM32F4VE_H

#ifdef __cplusplus
 extern "C" {
#endif
                                              
#include "stm32f4xx_hal.h"
   
/** 
* @brief  Define for STM32F4_DISCOVERY board  
#if !defined (USE_STM32F4_DISCO)
 #define USE_STM32F4_DISCO
#endif
*/ 

uint32_t BSP_GetVersion(void);
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4VE_H */

