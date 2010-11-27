/**
 * @file
 * @brief Test measure
 *
 * @date 27.11.2010
 * @author Anton Kozlov
 */

/**
 * Sleeps for some time,
 * then just print info of IRQ measures
 * on diag interface
 */

#include <embox/test.h>
#include <kernel/measure.h>

EMBOX_TEST(measure_test_run);

#define WAIT_TIME 1000/*ms to collect data*/

static void print_measure(const char *descr, measure_time_t *time) {
	TRACE("%s SYS:%2d DEV:%5d\n", descr, time->ticks, time->clocks);
}

static int measure_test_run(void) {
	usleep(WAIT_TIME);
	TRACE("\n");
	print_measure("OVR", &measure_overhead);
	print_measure("IR1", &irq_process[1]);

	return 0;
}

