/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */

#include <hal/reg.h>

#define UART_LSR_DR     0x01            /* Data ready */
#define UART_LSR_THRE   0x20            /* Xmit holding register empty */
#define COM3_BASE (0x49000000 + 0x20000)

#define UART_REG(x)                                                     \
        unsigned char x;                                                \
        unsigned char postpad_##x[3];

static struct com {
        UART_REG(rbr);          /* 0 */
        UART_REG(ier);          /* 1 */
        UART_REG(fcr);          /* 2 */
        UART_REG(lcr);          /* 3 */
        UART_REG(mcr);          /* 4 */
        UART_REG(lsr);          /* 5 */
        UART_REG(msr);          /* 6 */
        UART_REG(spr);          /* 7 */
        UART_REG(mdr1);         /* 8 */
        UART_REG(reg9);         /* 9 */
        UART_REG(regA);         /* A */
        UART_REG(regB);         /* B */
        UART_REG(regC);         /* C */
        UART_REG(regD);         /* D */
        UART_REG(regE);         /* E */
        UART_REG(uasr);         /* F */
        UART_REG(scr);          /* 10*/
        UART_REG(ssr);          /* 11*/
        UART_REG(reg12);        /* 12*/
        UART_REG(osc_12m_sel);  /* 13*/
};

#define COM3_RBR (((struct com *) COM3_BASE)->rbr)
#define COM3_LSR (((struct com *) COM3_BASE)->lsr)

static char diag_ns16550_getc(void) {

	while ((COM3_LSR & UART_LSR_DR) == 0);

	return COM3_RBR;

}

static void diag_ns16550_putc(char ch) {
	unsigned char *state = &COM3_LSR;

	state = state;

	while ((COM3_LSR & UART_LSR_THRE) == 0);

	COM3_RBR = ch;
}

static int diag_ns16550_kbhit(void) {
	return 0; /* TODO */
}

static const struct diag_ops diag_ns16550_ops = {
	.getc = &diag_ns16550_getc,
	.putc = &diag_ns16550_putc,
	.kbhit = &diag_ns16550_kbhit
};

void diag_init(void) {
	diag_common_set_ops(&diag_ns16550_ops);
}
