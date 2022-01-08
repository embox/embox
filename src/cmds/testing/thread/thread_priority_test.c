/**
 * @file
 *
 * @brief Testing interface for thread with different priority
 *
 * @date 2021.12.31
 * @author: kpishere
 */

#include <embox/test.h>

#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <framework/mod/options.h>

#include <pthread.h>

#include <util/err.h>
#include <util/log.h>

#define PRIORITY_INC 0

#define THREADS_QUANTITY OPTION_GET(NUMBER, threads_quantity)
#define __str(x) #x
#define str(x) __str(x)

//#define USE_PTHREAD 1

static void *thread_run(void *arg) {
	ksleep(2 * 1000);
	return 0;
}

int main(int argc, char **argv) {
    int priority = thread_self()->schedee.priority.current_priority;
#ifdef USE_PTHREAD
        pthread_t t[THREADS_QUANTITY];
        pthread_t res[THREADS_QUANTITY];
        int status;
#else
        void *res[THREADS_QUANTITY];
        struct thread *t[THREADS_QUANTITY];
#endif

	log_debug("Main process priority %d", priority);

	for (int i = 0; i < THREADS_QUANTITY; i++) {		

#ifdef USE_PTHREAD
        status = pthread_create(&t[i], NULL, thread_run, &res[i]);
        if (status != 0) {
            test_assert(0);
        }
#else
		t[i] = thread_create( THREAD_FLAG_SUSPENDED,
				thread_run, (void *) (uintptr_t) i);
		log_debug("Thread create %X %X",i, t[i]->schedee.priority.current_priority);
		test_assert_zero(err(t[i]));
		if(false) {
			// A thread that fails to schedule, delete it
			test_assert_zero(schedee_priority_set(&t[i]->schedee, i));
			log_debug("Thread cancel %X %X",i, t[i]->schedee.priority.current_priority);
			thread_cancel(t[i]);
		} else {
			test_assert_zero(schedee_priority_set(&t[i]->schedee, i+PRIORITY_INC));
			log_debug("Thread re-prioritize %X %X",i, t[i]->schedee.priority.current_priority);
		}
		test_assert_zero(thread_launch(t[i]));
#endif
	}
	for (int i = 0; i < THREADS_QUANTITY; i++) {
#ifdef USE_PTHREAD
        pthread_join(t[i], (void**)&status);
#else
		test_assert_zero(thread_join(t[i],&res[i]));
#endif        
		log_debug("Thread joined %d",i);
	}
    return 0;
}
