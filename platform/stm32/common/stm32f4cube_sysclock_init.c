#include <config/board_config.h>

#include <bsp/stm32cube_hal.h>

extern int RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct);

#if defined(CONF_RCC_TYPE_HSE)
# define RCC_OSCILLATORTYPE     RCC_OSCILLATORTYPE_HSE
# define RCC_PLLSOURCE          RCC_PLLSOURCE_HSE
#elif defined(CONF_RCC_TYPE_HSI)
# define RCC_OSCILLATORTYPE     RCC_OSCILLATORTYPE_HSI
# define RCC_PLLSOURCE          RCC_PLLSOURCE_HSI
#elif defined(CONF_RCC_TYPE_MSI)
# define RCC_OSCILLATORTYPE     RCC_OSCILLATORTYPE_MSI
# define RCC_PLLSOURCE          RCC_PLLSOURCE_MSI
#endif /* defined(CONF_RCC_TYPE_HSE) */

#if defined(CONF_RCC_TYPE_PLL) && defined(CONF_RCC_CLK_DEF_PLL_P_VAL)
# if (CONF_RCC_CLK_DEF_PLL_P_VAL == 1)
# define RCC_PLLP                    0
# elif (CONF_RCC_CLK_DEF_PLL_P_VAL == 2)
# define RCC_PLLP                    RCC_PLLP_DIV2
# elif (CONF_RCC_CLK_DEF_PLL_P_VAL == 4)
# define RCC_PLLP                    RCC_PLLP_DIV4
# elif (CONF_RCC_CLK_DEF_PLL_P_VAL == 8)
# define RCC_PLLP                    RCC_PLLP_DIV8
# else
# error "wrong PLL_P value"
# endif /* if (CONF_RCC_СLK_DEF_PLL_P_VAL == x) */
#endif /* defined(CONF_RCC_TYPE_PLL) && defined(CONF_RCC_СLK_DEF_PLL_P_VAL) */

#if defined(CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL)
# if (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 1)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV1
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 2)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV2
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 4)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV4
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 8)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV8
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 16)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV16
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 64)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV64
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 128)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV128
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 256)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV256
# elif (CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL == 512)
# define  RCC_AHB1_DIV     RCC_SYSCLK_DIV512
# else
# error "Wrong AHB_PRESCALER_VAL"
# endif
#endif /* defined(CONF_RCC_CLK_DEF_AHB_PRESCALER_VAL) */

#if defined(CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL)
# if (CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL == 1)
# define  RCC_APB1_DIV      RCC_HCLK_DIV1
# elif (CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL == 2)
# define  RCC_APB1_DIV      RCC_HCLK_DIV2
# elif (CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL == 4)
# define  RCC_APB1_DIV      RCC_HCLK_DIV4
# elif (CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL == 8)
# define  RCC_APB1_DIV      RCC_HCLK_DIV8
# elif (CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL == 16)
# define  RCC_APB1_DIV      RCC_HCLK_DIV16
# else
# error "Wrong AHB_PRESCALER_VAL"
# endif
#endif /* defined(CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL) */

#if defined(CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL)
# if (CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL == 1)
# define  RCC_APB2_DIV      RCC_HCLK_DIV1
# elif (CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL == 2)
# define  RCC_APB2_DIV      RCC_HCLK_DIV2
# elif (CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL == 4)
# define  RCC_APB2_DIV      RCC_HCLK_DIV4
# elif (CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL == 8)
# define  RCC_APB2_DIV      RCC_HCLK_DIV8
# elif (CONF_RCC_CLK_DEF_APB2_PRESCALER_VAL == 16)
# define  RCC_APB2_DIV      RCC_HCLK_DIV16
# else
# error "Wrong AHB_PRESCALER_VAL"
# endif
#endif /* defined(CONF_RCC_CLK_DEF_APB1_PRESCALER_VAL) */


void stm32_sysclock_init(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	/* Enable HSE Oscillator and activate PLL with HSE as source */

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE;
#if defined(CONF_RCC_TYPE_HSE)
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
#endif /* defined(CONF_RCC_TYPE_HSE) */
#if defined(CONF_RCC_TYPE_PLL)
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE;
	RCC_OscInitStruct.PLL.PLLM = CONF_RCC_CLK_DEF_PLL_M_VAL;
	RCC_OscInitStruct.PLL.PLLN = CONF_RCC_CLK_DEF_PLL_N_VAL;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP;
	RCC_OscInitStruct.PLL.PLLQ = CONF_RCC_CLK_DEF_PLL_Q_VAL;
#if defined(CONF_RCC_СLK_DEF_PLL_R_VAL)
	RCC_OscInitStruct.PLL.PLLR = CONF_RCC_CLK_DEF_PLL_R_VAL;
#endif /* defined(CONF_RCC_СLK_DEF_PLL_R_VAL) */
#endif /* defined(CONF_RCC_TYPE_PLL) */
	RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
	                               | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_AHB1_DIV;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

}
