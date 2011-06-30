/**
 * @file
 * @brief TODO This file is derived from Embox test template.
 *
 * @date 30.06.11
 * @author TODO Your name here
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <unistd.h>
#include <kernel/timer.h>

#define TIME_SLEEP	3000
#define NUM_THREADS 10

EMBOX_TEST(run);

/**
 * The test of sleep function. Test is presented as `sleep sort' algorithm.
 * if sleep function realy work right in end of execution the test we must see sorted(!) sequence of random numbers.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */

void* handler(void* args) {
	uint32_t id = (uint32_t) args;
	uint32_t now = cnt_system_time();

	printf("I'm %d thread... run\r\n", id);
	usleep(TIME_SLEEP);
	printf("I'm %d thread... done at %u (was started at %u, was running %u)\r\n",
			id, cnt_system_time(), now, TIME_SLEEP);
	return NULL;
}

static int run(void) {
	uint32_t i;
	struct thread *t[NUM_THREADS];

	printf("Sleep sort :)\n");

	for (i = 0; i < NUM_THREADS; i++) {
		thread_create(&t[i], THREAD_FLAG_DETACHED, &handler, (void *) i);
	}
#if 0
	for (i = 0; i < NUM_THREADS; i++) {
		thread_join(t[i], NULL);
	}
#endif
	return 0;
}

