/**
 * @file at91_diag_uart.c
 * @brief at91 diag uart
 *
 * @date 7.06.2010
 * @author Anton Kozlov
 */

#include <types.h>

#include <kernel/diag.h>
#include <hal/reg.h>

#define make_reg(offset) ((unsigned long *) (BASE_ADDR + offset))
#define BASE_ADDR 0xfffc0000
#define US_CR	  make_reg(0x0)
#define US_MR	  make_reg(0x4)
#define US_CSR	  make_reg(0x14)
#define US_RHR	  make_reg(0x18)
#define US_THR	  make_reg(0x1c)
#define US_BRGR	  make_reg(0x20)
#define US_TTGR	  make_reg(0x28)
#define PIO_PDR	  0xfffff404
#define PIO_ASR	  0xfffff470
#define PIO_BSR	  0xfffff474
#define PMC_PCER  0xfffffc10
#define PDC_PTCR  make_reg(0x120)

void diag_init(void) {
	REG_STORE(PIO_PDR, 0x60); //0x60
	REG_STORE(PIO_ASR, 0x60);
	REG_STORE(US_CR, 0xac); /*resetting & disabling RX, TX */
	REG_STORE(US_BRGR, 313); // for 9600
	REG_STORE(US_MR, 0x8c0);
	REG_STORE(PMC_PCER, 0x40);
	REG_STORE(US_TTGR, 0);
	//REG_STORE(PDC_PTCR, 11);
	REG_STORE(US_CR, 0x50); /*enabling RX, TX */
}

char diag_getc(void) {
	while (!diag_has_symbol()) {
	}
	return (char) REG_LOAD(US_RHR);
}

int diag_has_symbol(void) {
	return 0x1 & REG_LOAD(US_CSR);
}

void diag_putc(char ch) {
	while (!(0x2 & REG_LOAD(US_CSR))) {  //while (!line_is_accepts_new_char)
	}
	REG_STORE(US_THR, (unsigned long) ch);
}
