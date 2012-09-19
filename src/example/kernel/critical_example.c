/**
 * @file
 *
 * @brief Demonstrates wotk with "critical" API
 *
 * @date 26.08.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>
#include <kernel/softirq.h>
#include <kernel/printk.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

static void softirq_handler(unsigned int softirq_nr, void *data) {
	char *param_string = *((char **)data);
	printk("Softirq handler: %s\n", param_string);
}

static void locked_section(char **param_string) {
	int sleep_cnt;

	*param_string = "It's attempt in the locked section";

	softirq_lock();

	softirq_raise(SOFTIRQ_NR_TEST);
	for(sleep_cnt = 0; sleep_cnt < 1000000; sleep_cnt ++){
	}
	printk("Output from the locked section\n");

	softirq_unlock();
}

static void unlocked_section(char **param_string) {
	int sleep_cnt;

	*param_string = "It's attempt in the unlocked section";

	softirq_raise(SOFTIRQ_NR_TEST);
	for(sleep_cnt = 0; sleep_cnt < 1000000; sleep_cnt ++){
	}
	printk("Output from the unlocked section\n");
}

/**
 * Example's execution routing
 * It has been declare with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	char * param_string;

	softirq_install(SOFTIRQ_NR_TEST,  softirq_handler, (void *) &param_string);

	locked_section(&param_string);
	unlocked_section(&param_string);

	return ENOERR;
}
