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
#if 0
	variable->list_begin_convar = &idle_mutex->sleeped_thread_list;
#endif
}

int convar_wait(struct mutex *added_mutex, struct condition_variable *variable) {
	return scheduler_convar_wait(added_mutex, variable);
}

void convar_signal(struct condition_variable *variable) {
	scheduler_convar_signal(variable);
}

void convar_destroy(struct condition_variable *variable) {
	variable->list_begin_convar = NULL;
}
