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

#define WAIT_TIME 4000/*ms to collect data*/

static void print_measure(const char *descr, measure_time_t *time) {
	//TRACE("%s SYS:%2d DEV:%5d\n", descr, time->ticks, time->clocks);
	TRACE("%s %d %d\n", descr, time->ticks, time->clocks);
}

static measure_time_t get_time, send_time;

void avr_get_process(measure_time_t *time) {
	get_time = *time;
}

void avr_send_process(measure_time_t *time) {
	send_time = *time;
}

static int measure_test_run(void) {
	usleep(WAIT_TIME);
	TRACE("\n");
	print_measure("OVR", &measure_overhead);
	print_measure("GET", &get_time);
	print_measure("SND", &send_time);
	print_measure("PIO", &irq_process[2]);
	return 0;
}

