/**
 * @file main.c
 *
 * @date 27.11.08
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include <hal/arch.h>
#include <hal/ipl.h>
#include <kernel/irq.h>
#include <kernel/diag.h>
#include <kernel/timer.h>
#include <kernel/init.h>

int main(void) {
	arch_init();

	diag_init();
	irq_init();
#if 0
	timer_init();
#endif

	ipl_init();

	init();

	while(1) {
		arch_idle();
	}

	/* NOT REACHED */
	return 0;
}
