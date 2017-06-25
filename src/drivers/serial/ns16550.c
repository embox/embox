/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */

#include <hal/reg.h>
#include <drivers/diag.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mmu.h>
#include <mem/vmem.h>
#include <util/binalign.h>

#define UART_LSR_DR     0x01            /* Data ready */
#define UART_LSR_THRE   0x20            /* Xmit holding register empty */
#define COM_BASE (OPTION_GET(NUMBER, base_addr))

#define UART_REG(x)                                                     \
        unsigned char x;                                                \
        unsigned char postpad_##x[3];

struct com {
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

#define COM3 ((volatile struct com *)COM_BASE)
#define COM3_RBR (COM3->rbr)
#define COM3_LSR (COM3->lsr)

EMBOX_UNIT_INIT(ns16550_init);

static int ns16550_init(void) {
	/* Map one vmem page to handle this device if mmu is used */
	mmap_device_memory(
			(void*) (COM_BASE & ~MMU_PAGE_MASK),
			PROT_READ | PROT_WRITE | PROT_NOCACHE,
			binalign_bound(sizeof (struct com), MMU_PAGE_SIZE),
			MAP_FIXED,
			COM_BASE & ~MMU_PAGE_MASK
			);
	return 0;
}

static void ns16550_diag_putc(const struct diag *diag, char ch) {
	while ((COM3_LSR & UART_LSR_THRE) == 0);

	COM3_RBR = ch;
}

static char ns16550_diag_getc(const struct diag *diag) {
	return COM3_RBR;

}

static int ns16550_diag_kbhit(const struct diag *diag) {
	return COM3_LSR & UART_LSR_DR;
}

DIAG_OPS_DECLARE(
		.putc = ns16550_diag_putc,
		.getc = ns16550_diag_getc,
		.kbhit = ns16550_diag_kbhit,
);
