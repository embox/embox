/**
 * @file
 * @brief TODO This file is derived from Embox test template.
 *
 * @date
 * @author TODO Your name here
 */

#include <embox/test.h>
#include <kernel/thread/api.h>
#include <unistd.h>

#define TEST_SIZE 10
#define TEST_SLEEP 1
#define TEST_OF_TEST 0

EMBOX_TEST(run);

/**
 * The test of sleep function. Test is presented as `sleep sort' algorithm.
 * if sleep function realy work right in end of execution the test we must see sorted(!) sequence of random numbers.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */

int static_random_value = 177;

uint32_t random(void) {
	static_random_value *= 31;
	static_random_value += 11;
	return static_random_value;
}

void* handler(void* args) {
	uint32_t id = (uint32_t) args;
	#if TEST_SLEEP
	sleep(id);
	#else
	usleep(id);
	#endif
	printf("%d\r\n",id);
	return NULL;
}

static int run(void) {
	int result = 0;
	uint32_t i;

	printf("Sleep sort :)\n");

	for (i=0; i<TEST_SIZE; ++i) {
	#if TEST_OF_TEST
		handler((void*)(random()%TEST_SIZE));
	#else
		struct thread *p;
		thread_create(&p,THREAD_FLAG_DETACHED,&handler,(void*)(random()%TEST_SIZE));
	#endif
	}

	return result;
}

