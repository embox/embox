#include "rst_clk.h"
#include "mdr_uart.h"

#include "mdr1206fi.h"

#include <drivers/gpio.h>
extern int clk_enable(char *clk_name);

void UART_Config()
{
        UART_InitTypeDef UART_Print;
 #if 0
    PORT_InitTypeDef UART_Pins = {
        .PORT_Pin       = PORT_PIN_0,
        .PORT_Mode      = PORT_MODE_DIGITAL,
        .PORT_Direction = PORT_DIRECTION_INPUT,
        .PORT_Function  = PORT_FUNCTION_MAIN,
        .PORT_Power     = PORT_POWER_NOMINAL_UPTO_2mA,
        .PORT_PullUp = PORT_PULL_UP_OFF,
        .PORT_PullDown = PORT_PULL_DOWN_OFF,
    };


    //RST_CLK_PCLKCmd(RST_CLK_PCLK_PORTB, ENABLE);
    clk_enable("CLK_GPIOB");

    PORT_Init(MDR_PORTB, &UART_Pins);
    UART_Pins.PORT_Pin = PORT_PIN_1;
    //UART_Pins.PORT_Direction = PORT_DIRECTION_INPUT;
    PORT_Init(MDR_PORTB, &UART_Pins);
#endif

	gpio_setup_mode(1,
			(1 << 0),
			GPIO_MODE_ALT_SET(1));

	gpio_setup_mode(1,
			(1 << 1),
			GPIO_MODE_ALT_SET(1));

    RST_CLK_PER1_C2_ClkSelection(RST_CLK_PER1_C2_CLK_SRC_CPU_C1);
    RST_CLK_PER1_C2_SetPrescaler(RST_CLK_PER1_C2_UART1, RST_CLK_PER1_PRESCALER_DIV_1);
    RST_CLK_PER1_C2_Cmd(RST_CLK_PER1_C2_UART1, ENABLE);

    RST_CLK_PCLKCmd(RST_CLK_PCLK_UART1, ENABLE);

    UART_DeInit(MDR_UART1);
    UART_StructInit(&UART_Print);
    UART_Print.UART_BaudRate = 115200;
    UART_Init(MDR_UART1, &UART_Print);

    UART_Cmd(MDR_UART1, ENABLE);
}
