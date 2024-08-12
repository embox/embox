/**
 * @file
 * @brief Check consistency of FPU context
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 * @version
 * @date 28.03.2018
 */

#include <pthread.h>
#include <unistd.h> /* sleep() */

#include <embox/test.h>
#include <kernel/printk.h>

EMBOX_TEST_SUITE("FPU context consistency test. Must be compiled with -02");

#define TICK_COUNT 10

static float res_out[2][TICK_COUNT];

static void *fpu_context_thr1_hnd(void *arg) {
	float res = 1.0f;
	int i;

	for (i = 0; i < TICK_COUNT;) {
		res_out[0][i] = res;

		if (i == 0 || res_out[1][i - 1] > 0) {
			i++;
		}

		if (res > 0.000001f) {
			res /= 1.01f;
		}

		sleep(0);
	}

	return NULL;
}

static void *fpu_context_thr2_hnd(void *arg) {
	float res = 1.0f;
	int i = 0;

	for (i = 0; i < TICK_COUNT;) {
		res_out[1][i] = res;

		if (res_out[0][i] != 0) {
			i++;
		}

		if (res < 1000000.f) {
			res *= 1.01f;
		}

		sleep(0);
	}

	return NULL;
}

TEST_CASE("Test FPU context consistency") {
	pthread_t threads[2];
	pthread_t tid = 0;
	int status;

	status = pthread_create(&threads[0], NULL, fpu_context_thr1_hnd, &tid);
	if (status != 0) {
		test_assert(0);
	}

	status = pthread_create(&threads[1], NULL, fpu_context_thr2_hnd, &tid);
	if (status != 0) {
		test_assert(0);
	}

	pthread_join(threads[0], (void **)&status);
	pthread_join(threads[1], (void **)&status);

	test_assert(res_out[0][0] != 0 && res_out[1][0] != 0);

	for (int i = 1; i < TICK_COUNT; i++) {
		test_assert(res_out[0][i] < res_out[0][i - 1]);
		test_assert(res_out[1][i] > res_out[1][i - 1]);
	}
}
