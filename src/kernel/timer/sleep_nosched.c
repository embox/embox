/**
 * @file
 * @brief Single-thread sleep
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <types.h>
#include <unistd.h>
#include <kernel/irq.h>
#include <hal/clock.h>
#include <kernel/timer.h>
#include <time.h>
#include <util/array.h>
#include <string.h>
#include <embox/unit.h>
#include <lib/list.h>
#include <hal/arch.h>
#include <kernel/timer.h>

#define TIMER_FREQUENCY 1000

static int wait_flag; // for sleep func

static void restore_thread(uint32_t id) {
	wait_flag = 0;
	close_timer(id);
}

/*system library function */
int usleep(useconds_t usec) {
	uint32_t id;

	wait_flag = 1;
	if (!(id = get_free_timer_id()) || !set_timer(id, usec, &restore_thread)) {
		return 1;
	}
	while (wait_flag) {
		arch_idle();
	}
	return 0;
}

int sleep(uint32_t seconds) {
	return usleep(seconds * TIMER_FREQUENCY);
}
