/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    17.03.2015
 */

#include <time.h>
#include <stdlib.h>
#include <kernel/time/ktime.h>
#include <kernel/printk.h>

#define ITER_COUNT 10

static uint32_t ktime_exec_time;

static uint32_t div32time(void) {
	uint64_t t;
	uint32_t a = random();
	uint32_t b = random();

	t = ktime_get_ns();
	a = a / b;
	t = ktime_get_ns() - t - ktime_exec_time;

	return t;
}

static uint32_t div64time(void) {
	uint64_t t;
	uint64_t a = random();
	uint64_t b = random();

	t = ktime_get_ns();
	a = a / b;
	t = ktime_get_ns() - t - ktime_exec_time;

	return t;
}

static void calc_ktime_exec_time(void) {
	uint64_t t;

	/* calculate ktime_get_ns overhead  */
	t = ktime_get_ns();
	ktime_exec_time = ktime_get_ns() - t;
}

int main(int argc, char **argv) {
	int i = 0;

	calc_ktime_exec_time();

	printk("Execution time of 64 and 32 bit division in nanoseconds:\n"
			"   div64    div32\n\n");
	for(i = 0; i < ITER_COUNT; i++) {
		printk("%8d %8d\n", div64time(), div32time());
	}

	return 0;
}
