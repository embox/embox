/**
 * @file
 *
 * @brief
 *
 * @date 24.07.2012
 * @author Anton Bondarev
 */
#include <asm/traps.h>
#include <asm/ptrace.h>
#include <asm/mipsregs.h>
#include <assert.h>

#include <string.h>

typedef void (*exception_handler_t)(void);

extern void mips_first_exception_handler(void);
extern void mips_second_exception_handler(void);

exception_handler_t exception_handlers[32] = {mips_second_exception_handler};


void mips_c_exception_handler(pt_regs_t *regs) {

}

#define EBASE 0x80000000

/*
* Copy the generic exception handlers to their final destination.
* This will be overriden later as suitable for a particular
* configuration.
*/
void mips_exception_init(void) {
	unsigned int tmp;

	/* clear BEV bit */
	tmp = read_c0_status();
	tmp &= ~(ST0_BEV);
	write_c0_status(tmp);

	/* crear CauseIV bit */
	tmp = read_c0_cause();
	tmp &= ~(CAUSE_IV);
	write_c0_cause(tmp);

	memcpy((void *)(EBASE + 0x180), &mips_first_exception_handler, 0x80);
	for(tmp = 0; tmp < 32; tmp ++) {
		exception_handlers[tmp] = mips_second_exception_handler;
	}

}


