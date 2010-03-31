/**
 * @file
 * @brief Timer handling.
 */
#include <types.h>
#include <unistd.h>
#include <kernel/irq.h>
#include <hal/clock.h>
#include <hal/arch.h>
#include <kernel/timer.h>
#include <time.h>
#include <embox/kernel.h> /*for array_len*/
#include <string.h>

typedef struct sys_tmr {
	volatile int	f_enable;
	volatile uint32_t   id;
	volatile uint32_t   load;
	volatile uint32_t   cnt;
	volatile TIMER_FUNC handle;
} sys_tmr_t;

static volatile uint32_t cnt_ms_sleep; /**< for sleep function */
static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

static sys_tmr_t sys_timers[_SC_TIMER_MAX];

/**
 * Enable the timer.
 * @param num the timer index in the timers array
 */
static void set_sys_timer_enable (int num) {
	sys_timers[num].f_enable = 1;
}

/**
 * Disable the timer.
 * @param num the timer index in the timers array
 */
static void set_sys_timer_disable (int num) {
	sys_timers[num].f_enable = 0;
}

int set_timer (uint32_t id, uint32_t ticks, TIMER_FUNC handle) {
	int i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (!sys_timers[i].f_enable) {
			sys_timers[i].handle = handle;
			sys_timers[i].load = ticks;
			sys_timers[i].cnt = ticks;
			sys_timers[i].id = id;
			set_sys_timer_enable(i);
			return 1;
		}
	}
	return 0;
}

void close_timer (uint32_t id) {
	int i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (id == sys_timers[i].id) {
			sys_timers[i].f_enable = 0;
			sys_timers[i].handle = NULL;
		}
	}
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
static void inc_sys_timers (void) {
	int i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (sys_timers[i].f_enable && !(sys_timers[i].cnt--)) {
			sys_timers[i].cnt = sys_timers[i].load;
			if (sys_timers[i].handle) {
				sys_timers[i].handle(sys_timers[i].id);
			}
		}
	}
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	cnt_ms_sleep++;
	cnt_sys_time++;
	inc_sys_timers();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
int timer_init(void) {
	int i;
	cnt_sys_time = 0;
	for (i = 0; i < array_len(sys_timers); i++) {
		set_sys_timer_disable(i);
	}
	clock_init();
	clock_setup(1000);
	return 0;
}

int usleep(useconds_t usec) {
	cnt_ms_sleep = 0;

	while (cnt_ms_sleep < usec) {
		arch_idle();
	}
	return 0;
}

#if 0
/*TODO now save only one timer context*/
#define MAX_SAVE_CONTEXT	  2

static sys_tmr_t save_timers_buffer[_SC_TIMER_MAX][MAX_SAVE_CONTEXT];
static int save_context_number = 0;

int timers_context_save () {
	int context_number = 0;
	memcpy(save_timers_buffer[context_number], sys_timers,
				sizeof (sys_timers));
	return context_number;
}

int timers_context_restore (int context_number) {
	memcpy(sys_timers, save_timers_buffer[context_number],
							sizeof (sys_timers));
	return context_number;
}

void timers_off () {
	platform_timers_off ();
}
#endif
