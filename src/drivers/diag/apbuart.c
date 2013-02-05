/**
 * @file
 * @brief TODO
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <types.h>

#include <hal/reg.h>
#include <hal/system.h>
#include <drivers/diag.h>

#define APBUART_BASE  0x80000100
#define DATA_REG             0x0
#define STATUS_REG           0x4
#define CTRL_REG             0x8
#define SCALER_REG           0xc

#define BAUD_RATE \
	OPTION_GET(NUMBER, baud_rate)

#define SCALER_VAL \
	(((SYS_CLOCK * 10) / (BAUD_RATE * 8) - 5) / 10)

static char diag_apbuart_getc(void) {
	while (!(0x1 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
	}
	return ((char) REG_LOAD((volatile uint32_t *) (APBUART_BASE + DATA_REG)));
}

static void diag_apbuart_putc(char ch) {
	while (!(0x4 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
	}
	REG_STORE((volatile uint32_t *) (APBUART_BASE + DATA_REG), (uint32_t) ch);
}

static int diag_apbuart_kbhitl(void) {
	return (0x1 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)));
}

static const struct diag_ops diag_apbuart_ops = {
	.getc = &diag_apbuart_getc,
	.putc = &diag_apbuart_putc,
	.kbhit = &diag_apbuart_kbhit
};

void diag_init(void) {
	diag_common_set_ops(&diag_apbuart_ops);
	REG_STORE((volatile uint32_t *) (APBUART_BASE + SCALER_REG), SCALER_VAL);
	REG_STORE((volatile uint32_t *) (APBUART_BASE + CTRL_REG), 0x3);
}
