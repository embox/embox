#include <stm32f4discovery.conf.h>

CONFIG {
    /* uarts */
    uarts[2].status = DISABLED;   /* USART2 overlaps some SPI1 pins */
    uarts[6].status = ENABLED;    /* Use USART6 for console */

    /* spis */
    spis[1].status = DISABLED;    /* Overlaps UART2 pins, so off */
    spis[2].status = DISABLED;    /* Overlaps Ethernet pins */
}
