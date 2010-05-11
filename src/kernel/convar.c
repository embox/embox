/**
 * @file
 * @brief Defines methods to work with condition variables.
 *
 * @date 11.05.2010
 * @author Alexey Kryachko
 */

#include <kernel/scheduler.h>

int convar_add_sleep(struct thread *added_thread) {
	return scheduler_add_sleep(added_thread);
}

void convar_wake_up(void) {
	scheduler_wake_up();
}
