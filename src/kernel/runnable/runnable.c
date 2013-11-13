/*

*/

#include <assert.h>

#include <kernel/thread.h>
#include <kernel/plannable/plannable.h>
#include <kernel/light_thread/light_thread.h>

struct plannable * plannable_create_thread(unsigned int flags,
		void *(*run)(void *), void *arg){

	assert(run);
	struct plannable result;

	result.type = NORMAL_THREAD;
	result.t = thread_create(flags, run, arg);

	result.sched_attr = &(result.t.sched_attr);
	result.policy = &(result.t.policy);

	return *result;
}

struct plannable * plannable_create_lthread(void *(*run)(void *), void *arg){
	assert(run);
	struct plannable result;

	result.type = LIGHT_THREAD;
	result.lt = lthread_create(run, arg);

	result.sched_attr = &(result.t.sched_attr);
	result.policy = &(result.t.policy);

	return *result;
}

