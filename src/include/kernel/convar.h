/**
 * @file
 * @brief Defines methods to work with condition variables.
 *
 * @date 11.05.2010
 * @author Alexey Kryachko
 */

#ifndef CONVAR_H_
#define CONVAR_H_

#include <kernel/mutex.h>

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
extern void convar_init(void);

/**
 * Add a thread to the list of sleeping threads.
 * @param added_mutex is mutex for sleeping.
 * @retval result of scheduler_add_sleep.
 */
extern int convar_wait(struct mutex *added_mutex, struct condition_variable *variable);

/**
 * Wake up of sleeping threads.
 */
extern void convar_signal(struct condition_variable *variable);

/**
 * Destroy of condition variable.
 */
extern void convar_destroy(struct condition_variable *variable);

#endif /* CONVAR_H_ */
