/**
 * @file
 * @brief
 *
 * @date 19.11.12
 * @author Anton Bulychev
 */

#include <errno.h>
#include <assert.h>

#include <kernel/usermode.h>
#include <kernel/thread/api.h>

int create_usermode_thread(struct thread **t, unsigned int flags,
		void *ip, void *sp) {

	struct ue_data *d;
	int fl = flags & THREAD_FLAG_SUSPENDED;
	int err;
	int data_size = sizeof(struct ue_data);

	if ((err = thread_create(t, fl, (void * (*)(void *)) usermode_entry, NULL))) {
		return err;
	}

	if (!(flags & THREAD_FLAG_SUSPENDED)) {
		thread_launch(*t);
	}

	/* Allocate usermode data on the stack */
	d = (struct ue_data *) (*t)->stack;
	d->ip = ip;
	d->sp = sp;

	(*t)->stack += data_size;
	(*t)->stack_sz -= data_size;
	context_set_stack(&(*t)->context, (*t)->stack + (*t)->stack_sz);

	/* Pass pointer to usermode entry data as argument */
	(*t)->run_arg = d;

	return ENOERR;
}
