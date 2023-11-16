/**
 * @file
 * @brief The kernel initialization sequence.
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <drivers/diag.h>
#include <drivers/irqctrl.h>
#include <embox/runlevel.h>
#include <hal/arch.h>
#include <hal/ipl.h>
#include <kernel/kgdb.h>
#include <kernel/klog.h>
#include <kernel/printk.h>
#include <util/log.h>

static void kernel_init(void);
static int init(void);
extern int system_start(void);
extern int system_abs_time_init(void);

/**
 * The setup of the system, the run level and execution of the idle function.
 */
void kernel_start(void) {
	kernel_init();

	kgdb_start(init);

	init();

	system_abs_time_init();

	system_start();

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

	printk("\n");

	klog_init();

	irqctrl_init();
	log_info("Interrupt controller: %s", irqctrl_get()->name);
}

/**
 * Set the run level to the value of the file system and net level.
 * @return 0
 */
static int init(void) {
	int ret;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_ENABLED - 1;

	log_info("Embox kernel start");

	ret = runlevel_set(target_level);

	return ret;
}
