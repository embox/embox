/**
 * @file
 * @brief Defines methods to work with condition variables.
 *
 * @date 11.05.2010
 * @author Alexey Kryachko
 */

#include <kernel/scheduler.h>
#include <kernel/convar.h>

void canvar_init(struct condition_variable *variable) {
	variable->list_begin_convar = &idle_thread->sleep_list;
}

int convar_add_sleep(struct thread *added_thread, struct condition_variable *variable) {
	scheduler_add_sleep(added_thread, variable);
}

void convar_wake_up(struct condition_variable *variable) {
	scheduler_wake_up(variable);
}
