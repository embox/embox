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

static uint32_t random_val = 54446;

uint32_t get_rand(void) {
	random_val *= 213355431;
	random_val -= 222287321;
	return random_val;
}

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
	uint32_t time1, time2;
	uint32_t sleep = ((get_rand() / 100000) * 100) % 10000;

	printf("Thread %d... run\n", id);
	time1 = cnt_system_time();
	usleep(sleep);
	time2 = cnt_system_time();
	printf("Thread %d... done at %u (was started at %u, was running %u)\n",
			id, time2, time1, sleep);
	return NULL;
}

static int run(void) {
	uint32_t i;
	struct thread *t[NUM_THREADS];

	printf("Sleep sort :)\n");

	for (i = 0; i < NUM_THREADS; i++) {
		thread_create(&t[i], 0, &handler, (void *) i);
	}
	for (i = 0; i < NUM_THREADS; i++) {
		thread_join(t[i], NULL);
	}
	return 0;
}

