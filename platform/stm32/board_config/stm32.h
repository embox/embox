#ifndef STM32_H_
#define STM32_H_

#define PA     GPIOA
#define PB     GPIOB
#define PC     GPIOC
#define PD     GPIOD
#define PE     GPIOE
#define PF     GPIOF
#define PG     GPIOG
#define PH     GPIOH
#define PI     GPIOI
#define PJ     GPIOJ
#define PK     GPIOK

#define PIN_0   GPIO_PIN_0
#define PIN_1   GPIO_PIN_1
#define PIN_2   GPIO_PIN_2
#define PIN_3   GPIO_PIN_3
#define PIN_4   GPIO_PIN_4
#define PIN_5   GPIO_PIN_5
#define PIN_6   GPIO_PIN_6
#define PIN_7   GPIO_PIN_7
#define PIN_8   GPIO_PIN_8
#define PIN_9   GPIO_PIN_9
#define PIN_10  GPIO_PIN_10
#define PIN_11  GPIO_PIN_11
#define PIN_12  GPIO_PIN_12
#define PIN_13  GPIO_PIN_13
#define PIN_14  GPIO_PIN_14
#define PIN_15  GPIO_PIN_15

#define AF0    0x0
#define AF1    0x1
#define AF2    0x2
#define AF3    0x3
#define AF4    0x4
#define AF5    0x5
#define AF6    0x6
#define AF7    0x7
#define AF8    0x8
#define AF9    0x9
#define AF10   0xa
#define AF11   0xb
#define AF12   0xc
#define AF13   0xd
#define AF14   0xe
#define AF15   0xf
#define NOAF   -1 /* No alternate */

/* GPIO clocks */
#define CLK_GPIOA     __HAL_RCC_GPIOA_CLK_ENABLE()
#define CLK_GPIOB     __HAL_RCC_GPIOB_CLK_ENABLE()
#define CLK_GPIOC     __HAL_RCC_GPIOC_CLK_ENABLE()
#define CLK_GPIOD     __HAL_RCC_GPIOD_CLK_ENABLE()
#define CLK_GPIOE     __HAL_RCC_GPIOE_CLK_ENABLE()
#define CLK_GPIOF     __HAL_RCC_GPIOF_CLK_ENABLE()
#define CLK_GPIOG     __HAL_RCC_GPIOG_CLK_ENABLE()
#define CLK_GPIOI     __HAL_RCC_GPIOI_CLK_ENABLE()
#define CLK_GPIOJ     __HAL_RCC_GPIOJ_CLK_ENABLE()
#define CLK_GPIOK     __HAL_RCC_GPIOK_CLK_ENABLE()

/* UART clocks */
#define CLK_USART1    __HAL_RCC_USART1_CLK_ENABLE()
#define CLK_USART2    __HAL_RCC_USART2_CLK_ENABLE()
#define CLK_USART3    __HAL_RCC_USART3_CLK_ENABLE()
#define CLK_UART4     __HAL_RCC_UART4_CLK_ENABLE()
#define CLK_UART5     __HAL_RCC_UART5_CLK_ENABLE()
#define CLK_USART6    __HAL_RCC_USART6_CLK_ENABLE()
#define CLK_UART7     __HAL_RCC_UART7_CLK_ENABLE()
#define CLK_UART8     __HAL_RCC_UART8_CLK_ENABLE()

/* SPI clocks */
#define CLK_SPI1      __HAL_RCC_SPI1_CLK_ENABLE()
#define CLK_SPI2      __HAL_RCC_SPI2_CLK_ENABLE()
#define CLK_SPI3      __HAL_RCC_SPI3_CLK_ENABLE()
#define CLK_SPI4      __HAL_RCC_SPI4_CLK_ENABLE()
#define CLK_SPI5      __HAL_RCC_SPI5_CLK_ENABLE()
#define CLK_SPI6      __HAL_RCC_SPI6_CLK_ENABLE()

/* I2C clocks */
#define CLK_I2C1      __HAL_RCC_I2C1_CLK_ENABLE()
#define CLK_I2C2      __HAL_RCC_I2C2_CLK_ENABLE()
#define CLK_I2C3      __HAL_RCC_I2C3_CLK_ENABLE()
#define CLK_I2C4      __HAL_RCC_I2C4_CLK_ENABLE()
#define CLK_I2C5      __HAL_RCC_I2C5_CLK_ENABLE()
#define CLK_I2C6      __HAL_RCC_I2C6_CLK_ENABLE()

/* TIM clocks */
#define CLK_TIM1      __HAL_RCC_TIM1_CLK_ENABLE()
#define CLK_TIM2      __HAL_RCC_TIM2_CLK_ENABLE()
#define CLK_TIM3      __HAL_RCC_TIM3_CLK_ENABLE()
#define CLK_TIM4      __HAL_RCC_TIM4_CLK_ENABLE()

/* TIM channels */
#define CHANNEL_TIM1      TIM_CHANNEL_1
#define CHANNEL_TIM2      TIM_CHANNEL_2
#define CHANNEL_TIM3      TIM_CHANNEL_3
#define CHANNEL_TIM4      TIM_CHANNEL_4

#define STM32_CLK(clk)  __HAL_RCC_ ## clk ## _CLK
#define STM32_CLK_ENABLE(clk)  __HAL_RCC_ ## clk ## _CLK_ENABLE


#endif /* STM32_H_ */
