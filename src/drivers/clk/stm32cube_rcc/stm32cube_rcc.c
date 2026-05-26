/**
 * @file
 *
 * @date 31.03.2017
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <bsp/stm32cube_hal.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO     "CLK_GPIO"
#define CLK_NAME_UART     "CLK_UART"
#define CLK_NAME_SPI      "CLK_SPI"
#define CLK_NAME_I2C      "CLK_I2C"
#define CLK_NAME_TMR      "CLK_TMR"

int stm32cube_rcc_tmr_en(int num) {
  switch(num) {
    case 1:
      __HAL_RCC_TIM1_CLK_ENABLE();
      return 0;
    case 2:
      __HAL_RCC_TIM2_CLK_ENABLE();
      return 0;
    case 3:
      __HAL_RCC_TIM3_CLK_ENABLE();
      return 0;
    case 4:
      __HAL_RCC_TIM4_CLK_ENABLE();
      return 0;
    default:
      return -ENOTSUP;
  }

  return -ENOTSUP;
}

int stm32cube_rcc_gpio_en(int num) {
  return 0;
}

int stm32cube_rcc_uart_en(int num) {
  return 0;
}

int clk_enable(char *clk_name) {
    int num;

    if (0 == strncmp(clk_name, CLK_NAME_GPIO, sizeof(CLK_NAME_GPIO) - 1)) {
        num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - 'A';
        stm32cube_rcc_gpio_en(num);
        return 0;
    }
    if (0 == strncmp(clk_name, CLK_NAME_UART, sizeof(CLK_NAME_UART) - 1)) {
        num = clk_name[sizeof(CLK_NAME_UART) - 1]  - '0';
        stm32cube_rcc_uart_en(num);
        return 0;
    }
    if (0 == strncmp(clk_name, CLK_NAME_TMR, sizeof(CLK_NAME_TMR) - 1)) {
        num = clk_name[sizeof(CLK_NAME_TMR) - 1]  - '0';
        stm32cube_rcc_tmr_en(num);
        return 0;
    }

    return -ENOSUPP;
}
