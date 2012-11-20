/**
 * @file
 * @brief
 *
 * @date 19.11.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_USERMODE_H_
#define KERNEL_THREAD_USERMODE_H_

#include <types.h>

#include <kernel/thread/api.h>

/* Usermode entry data */
struct ue_data {
	void *ip; // instruction pointer
	void *sp; // stack pointer
};


extern void usermode_entry(struct ue_data *data);
extern int create_usermode_thread(struct thread **p_thread, unsigned int flags,
		void *ip, void *sp);
extern int create_usermode_task(void *ip, void *sp);



#endif /* KERNEL_THREAD_USERMODE_H_ */
