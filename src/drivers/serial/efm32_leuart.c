/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.03.2014
 */

/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 * @author Energy Micro AS
 * @version 3.20.3
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/

#include <stdint.h>

/*#include <drivers/gpio.h>*/
#include <hal/reg.h>
#include <hal/system.h>
#include <drivers/diag.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>

#include <drivers/serial/uart_device.h>

#include <em_leuart.h>
#include <em_cmu.h>
#include <em_gpio.h>

#define BSP_BCC_LEUART        LEUART0
#define BSP_BCC_CLK           cmuClock_LEUART0
#define BSP_BCC_LOCATION      LEUART_ROUTE_LOCATION_LOC0
#define BSP_BCC_TXPORT        gpioPortD
#define BSP_BCC_TXPIN         4
#define BSP_BCC_RXPORT        gpioPortD
#define BSP_BCC_RXPIN         5
#define BSP_BCC_ENABLE_PORT   gpioPortA
#define BSP_BCC_ENABLE_PIN    9

static int efm32_uart_putc(struct uart *dev, int ch) {
	LEUART_Tx((void *) dev->base_addr, ch);
	return 0;
}

static int efm32_uart_hasrx(struct uart *dev) {
#if 1
static unsigned int neg_cnt;
	while (1)
		if (!GPIO_PinInGet(BSP_BCC_RXPORT, BSP_BCC_RXPIN)) {
			LEUART_Tx((void *) dev->base_addr, '0');
			neg_cnt++;
		}
#endif
	return 1;
}

static int efm32_uart_getc(struct uart *dev) {
	return LEUART_Rx((void *) dev->base_addr);
}

static int efm32_uart_setup(struct uart *dev, const struct uart_params *params) {

	LEUART_TypeDef      *leuart = (void *) dev->base_addr;
	LEUART_Init_TypeDef init    = LEUART_INIT_DEFAULT;

	/* Enable CORE LE clock in order to access LE modules */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Enable CORE LE clock in order to access LE modules */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Enable CORE LE clock in order to access LE modules */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Select LFXO for LEUARTs (and wait for it to stabilize) */
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

	CMU_ClockEnable(cmuClock_LEUART0, true);

	/* Do not prescale clock */
	CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

	/* Configure GPIO pin for UART TX */
	/* To avoid false start, configure output as high. */
	GPIO_PinModeSet( BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1 );
	/* Configure GPIO pin for UART RX */
	GPIO_PinModeSet( BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 1 );

	/* Configure LEUART */
	init.enable = leuartDisable;
	init.baudrate = 9600;
	LEUART_Init(leuart, &init);

	/* Enable the switch that enables UART communication. */
	GPIO_PinModeSet( BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1 );
	BSP_BCC_LEUART->ROUTE |= LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | BSP_BCC_LOCATION;

	/* Finally enable it */
	LEUART_Enable(leuart, leuartEnable);

	return 0;
}

static const struct uart_ops efm32_uart_ops = {
		.uart_getc = efm32_uart_getc,
		.uart_putc = efm32_uart_putc,
		.uart_hasrx = efm32_uart_hasrx,
		.uart_setup = efm32_uart_setup,
};

static struct uart efm32_uart0 = {
		.uart_ops = &efm32_uart_ops,
		.irq_num = 0,
		.base_addr = (unsigned long) LEUART0,
};

static const struct uart_params uart_defparams = {
		.baud_rate = 115200,
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

DIAG_SERIAL_DEF(&efm32_uart0, &uart_defparams);
