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
 * Add a thread to the list of sleeping threads.
 * @param removed_thread deleted thread.
 * @retval result of scheduler_add_sleep.
 */
int convar_add_sleep(struct thread *added_thread);

/**
 * Wake up of sleeping threads.
 */
void convar_wake_up(void);

#endif /* CONVAR_H_ */
