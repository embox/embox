/**
 * @file
 *
 * @brief
 *
 * @date 2024.8.7
 * @author Zeng Zixian
 */

#include <embox/test.h>

#include <util/log.h>

#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/thread/thread_stack.h>

#include <module/embox/kernel/stack.h>

#define MAX_TRYTIMES    ((unsigned int)(-1) - 1)

EMBOX_TEST_SUITE("test simultaneous thread on CPUs");

static unsigned int try_time = 0;
static unsigned int thread_status[4] = {0};

static int search_threads(void) {
	struct thread * th;
	struct task *task;
	int ret = 0;

	task_foreach(task) {
		task_foreach_thread(th, task) {
			if(th->schedee.active == 1 && th != thread_self()){
				ret = 1;
				thread_status[0] = th->schedee.active;
				thread_status[1] = th->schedee.ready;
				thread_status[2] = th->schedee.waiting;
				thread_status[3] = (unsigned int)th;
				return ret;
			}
		}
	}

	return ret;
}


void * test_thread_1(void *arg){
	struct thread * self = thread_self();
	int founded = 0;
	test_assert(self->schedee.active);

	while(try_time++ < MAX_TRYTIMES) {
		founded = search_threads();
		if(founded) break;
	}

	log_debug("found %d try_time %u",founded,try_time);
	if(founded){
		log_debug("Self thread %#x A %u R %u W %u", (unsigned int)self, self->schedee.active, self->schedee.ready, self->schedee.waiting);
		log_debug("Found thread %#x A %u R %u W %u", thread_status[3], thread_status[0], thread_status[1], thread_status[2]);
	}

	test_assert_not_zero(founded);
	return 0;
}

void * test_thread_2(void * arg){
	test_assert_zero(thread_join((struct thread *)arg,NULL));
	return 0;
}


TEST_CASE("two threads exsit simultaneously") {
	struct thread * thread1, * thread2;
	thread1 = thread_create(THREAD_FLAG_PRIORITY_HIGHER,test_thread_1,NULL);
	thread2 = thread_create(0,test_thread_2,thread1);
	thread_join(thread2,NULL);
}
