/**
 * @file
 * @brief TODO
 *
 * @date 16.02.2010
 * @author Eldar Abusalimov
 */

#include <errno.h>

#include <kernel/timer.h>

int set_timer(uint32_t id, uint32_t ticks, TIMER_FUNC handle) {
	return -ENOSYS;
}

void close_timer(uint32_t id) {
}

int timers_context_save(void) {
	return -ENOSYS;
}

int timers_context_restore(int context_number) {
	return -ENOSYS;
}

void timers_off(void) {
}
