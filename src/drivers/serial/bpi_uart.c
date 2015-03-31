/**
 * @file
 * @brief
 *
 * @author  Dmitrii Petukhov
 * @date    28.03.2015
 */

#include <hal/reg.h>
#include <drivers/diag.h>
#include <framework/mod/options.h>

#define UART_BASE (OPTION_GET(NUMBER, base_addr))

#define UART_REG(x)                                                     \
        unsigned char x;                                                \
        unsigned char postpad_##x[3];

struct uart_bpi {
	union {                      /* 0x8000(3) */
		UART_REG(rbr);
		UART_REG(thr);
	} rt;
	uint32_t unused[4];          /* 0x8004(13) */
	UART_REG(lsr);               /* 0x8014 */
};

#define UART ((volatile struct uart_bpi *)UART_BASE)
#define UART_RBR (UART->rt.rbr)
#define UART_LSR (UART->lsr)

#define LSR_FLAG_DR   (1 << 0)
#define LSR_FLAG_THRE (1 << 5)

static void bpi_diag_putc(const struct diag *diag, char ch) {
	while (!(REG_LOAD(&UART_LSR) & LSR_FLAG_THRE));
	REG_STORE(&UART_RBR, ch);
}

static char bpi_diag_getc(const struct diag *diag) {
	return REG_LOAD(&UART_RBR);
}

static int bpi_diag_kbhit(const struct diag *diag) {
	return REG_LOAD(&UART_LSR) & LSR_FLAG_DR;
}

DIAG_OPS_DECLARE(
		.putc  = bpi_diag_putc,
		.getc  = bpi_diag_getc,
		.kbhit = bpi_diag_kbhit,
);
