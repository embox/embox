/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <types.h>

#include <kernel/diag.h>
#include <hal/reg.h>

#define APBUART_BASE  0x80000100
#define DATA_REG             0x0
#define STATUS_REG           0x4
#define CTRL_REG             0x8
#define SCALER_REG           0xc

#define SCALER_VAL \
	((((CONFIG_CORE_FREQ*10) / (8 * CONFIG_UART_BAUD_RATE))-5)/10)

void diag_init(void) {
	REG_STORE((volatile uint32_t *) (APBUART_BASE + SCALER_REG), SCALER_VAL);
	REG_STORE((volatile uint32_t *) (APBUART_BASE + CTRL_REG), 0x3);
}

char diag_getc(void) {
	while (!(0x1 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
	}
	return ((char) REG_LOAD((volatile uint32_t *) (APBUART_BASE + DATA_REG)));
}

void diag_putc(char ch) {
	volatile int i;
	for (i = 0; i < 0x1000; i++) {
	}
//	while (!(0x4 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
//	}
	REG_STORE((volatile uint32_t *) (APBUART_BASE + DATA_REG), (uint32_t) ch);
}
