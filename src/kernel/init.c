/**
 * @file
 * @brief The kernel initialization sequence.
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <hal/arch.h>
#include <hal/ipl.h>
#include <drivers/diag.h>
#include <embox/runlevel.h>
#include <kernel/printk.h>

static void kernel_init(void);
static int init(void);

/**
 * The setup of the system, the run level and execution of the idle function.
 */
void kernel_start(void) {

	kernel_init();

	init();

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

	ipl_init();

	diag_init();
}

/**
 * Set the run level to the value of the file system and net level.
 * @return 0
 */
static int init(void) {
	int ret;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_TOTAL - 1;

	printk("\nEmbox kernel start\n");

	ret = runlevel_set(target_level);

	return ret;
}
