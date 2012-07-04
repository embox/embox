/**
 * @file
 *
 * @brief MIPS build-in interrupt controller
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#include <embox/unit.h>
#include <asm/mipsregs.h>

EMBOX_UNIT_INIT(unit_init);

/**
 * Initialize MIPS build-in interrupts controller
 */
static int unit_init(void) {
	write_c0_status(~ST0_IM);
//	write_c0_cause(~CAUSEF_IP);
	return 0;
}
