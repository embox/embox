#include "rst_clk.h"
#include "mdr_uart.h"
#include "mdr_gpio.h"
#include "mdr1206fi.h"


void UART_Config()
{
    PORT_InitTypeDef UART_Pins = {
        .PORT_Pin       = PORT_PIN_0,
        .PORT_Mode      = PORT_MODE_DIGITAL,
        .PORT_Direction = PORT_DIRECTION_OUTPUT,
        .PORT_Function  = PORT_FUNCTION_MAIN,
        .PORT_Power     = PORT_POWER_NOMINAL_UPTO_2mA,
        .PORT_PullUp = PORT_PULL_UP_OFF,
        .PORT_PullDown = PORT_PULL_DOWN_OFF,
    };
    UART_InitTypeDef UART_Print;

    RST_CLK_PCLKCmd(RST_CLK_PCLK_PORTB, ENABLE);

    PORT_Init(MDR_PORTB, &UART_Pins);
    UART_Pins.PORT_Pin = PORT_PIN_1;
    UART_Pins.PORT_Direction = PORT_DIRECTION_INPUT;
    PORT_Init(MDR_PORTB, &UART_Pins);

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
