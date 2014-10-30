

#ifndef ST_F4_STM32F4XX_CONF_H_
#define ST_F4_STM32F4XX_CONF_H_

#include <assert.h>
#include <framework/mod/options.h>

#include <misc.h>
#include <stm32f4xx_adc.h>
#include <stm32f4xx_can.h>
#include <stm32f4xx_crc.h>
#include <stm32f4xx_cryp.h>
#include <stm32f4xx_dac.h>
#include <stm32f4xx_dbgmcu.h>
#include <stm32f4xx_dcmi.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_flash.h>
#include <stm32f4xx_fsmc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_hash.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_iwdg.h>
#include <stm32f4xx_pwr.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_rng.h>
#include <stm32f4xx_rtc.h>
#include <stm32f4xx_sdio.h>
#include <stm32f4xx_spi.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_wwdg.h>

#define USE_FULL_ASSERT
#define assert_param(expr) assert(expr)

#include <module/third_party/bsp/st_f4.h>

#undef  HSE_VALUE
#define HSE_VALUE OPTION_MODULE_GET(third_party__bsp__st_f4, NUMBER, hse_freq_hz)

#endif /* ST_F4_STM32F4XX_CONF_H_ */
