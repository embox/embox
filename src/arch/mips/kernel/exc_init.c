/**
 * @file
 * @brief
 *
 * @date 24.07.2012
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <string.h>

#include <asm/addrspace.h>
#include <asm/mipsregs.h>
#include <asm/system.h>

extern void mips_first_exception_handler(void);
extern void flush_cache(unsigned long start_addr, size_t size);

void mips_exception_init(void) {
	unsigned int tmp;

	/* copy first exception handler to correct place */
	memcpy((void *)(KSEG0 + 0x180), &mips_first_exception_handler, 0x80);

	flush_cache((unsigned long)(KSEG0 + 0x180), 0x80);

#if __mips_isa_rev >= 2
	/* explicitly set ebase */
	mips_write_c0_ebase(KSEG0);
#endif

	execution_hazard_barrier();

	/* Configure status register */
	tmp = mips_read_c0_status();
	tmp &= ~(ST0_IM | ST0_BEV | ST0_ERL | ST0_IE);
	mips_write_c0_status(tmp);

	/* Configure cause register */
	tmp = mips_read_c0_cause();
	tmp &= ~(CAUSE_IV | CAUSE_IM);
	mips_write_c0_cause(tmp);
}
