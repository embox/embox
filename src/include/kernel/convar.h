/**
 * @file
 * @brief Defines methods to work with condition variables.
 *
 * @date 11.05.2010
 * @author Alexey Kryachko
 */

#ifndef CONVAR_H_
#define CONVAR_H_

#include <kernel/thread.h>

/**
 * Struct of condition variable.
 */
struct condition_variable {
	/** Begin of the list. */
	struct list_head *list_begin_convar;
};

/**
 * initialization of condition variable.
 */
void convar_init(void);

/**
 * Add a thread to the list of sleeping threads.
 * @param removed_thread deleted thread.
 * @retval result of scheduler_add_sleep.
 */
int convar_add_sleep(struct thread *added_thread, struct condition_variable *variable);

/**
 * Wake up of sleeping threads.
 */
void convar_wake_up(struct condition_variable *variable);

#endif /* CONVAR_H_ */
