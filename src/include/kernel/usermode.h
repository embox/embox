/**
 * @file
 * @brief
 *
 * @date 19.11.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_USERMODE_H_
#define KERNEL_THREAD_USERMODE_H_

#include <kernel/thread.h>

/* Usermode entry data */
struct ue_data {
	void *ip; // instruction pointer
	void *sp; // stack pointer
};

/* Architecture-dependent functions */

/*
 * Enter to usermode.
 *
 * @param data
 *   Usermode parameters (ip, sp).
 */
extern void usermode_entry(struct ue_data *data);

/*
 * Only switches to new stack.
 *
 * @param sp
 *   New stack pointer.
 */
extern void usermode_set_stack(void *sp);

/* Architecture-independent functions */

/*
 * Creates new usermode thread.
 *
 * @param ip
 *   First instruction pointer of the thread.
 * @param sp
 *   New stack pointer.
 *
 * Other information see in: include/kernel/thread.h
 */
extern int user_thread_create(struct thread **p_thread, unsigned int flags,
		void *ip, void *sp);

/*
 * Creates new usermode task.
 *
 * @param ip
 *   First instruction pointer of the task.
 * @param sp
 *   New stack pointer.
 *
 * Other information see in: include/kernel/task.h
 */
extern int user_task_create(void *ip, void *sp);

#endif /* KERNEL_THREAD_USERMODE_H_ */
