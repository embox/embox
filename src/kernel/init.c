/**
 * @file
 *
 * @brief The kernel initialization sequence.
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <embox/kernel.h>

#include <hal/arch.h>
#include <hal/ipl.h>
#include <kernel/irq.h>
#include <kernel/softirq.h>
#include <kernel/timer.h>
#include <kernel/diag.h>
#include <embox/runlevel.h>

void main(void);
static void kernel_init(void);
static int init(void);
// XXX remove from here. -- Eldar
int uart_init(void);

/**
 * The setup of the system, the run level and execution of the idle function.
 */
void kernel_start(void) {

	kernel_init();
	init();

	main();

	while (1) {
		arch_idle();
	}
}

/**
 * The initialization functions are called to set up interrupts, perform
 * further memory configuration, initialization of drivers, devices.
 */
static void kernel_init(void) {
	arch_init();

	diag_init();
	irq_init();
	softirq_init();

	uart_init(); // XXX

	ipl_init();

}

/**
 * Set the run level to the value of the file system and net level.
 * @return 0
 */
static int init(void) {
	int ret = 0;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_TOTAL - 1;

	TRACE("EMBOX kernel start\n");

	if (0 != (ret = runlevel_set(target_level))) {
		TRACE("Failed to get into level %d, current level %d\n", target_level,
				runlevel_get_entered());
	}
	return ret;
}
