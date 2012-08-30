/**
 * @file
 * @brief Tests mutex lock/unlock methods.
 *
 * @details The test creates two threads: both of them is created with
 *      (suspend) flag THREAD_FLAG_SUSPENDED. Than first thread is waked up
 *      and it is executing while it will not finish. The test checks a correct
 *      sequence of stages.
 *
 *
 * @date Apr 18, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/mutex.h>
#include <unistd.h>

static struct thread *first, *second;
static struct mutex m;

EMBOX_TEST_SUITE("Mutex test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_resume(first));
	test_assert_zero(thread_join(first, NULL));
	test_assert_zero(thread_join(second, NULL));
	test_assert_emitted("abc");
}

static void *first_run(void *arg) {
	mutex_lock(&m);
	test_emit('a');
	test_assert_zero(thread_resume(second));
	usleep(1000);
	test_emit('b');
	mutex_unlock(&m);
	return NULL;
}

static void *second_run(void *arg) {
	mutex_lock(&m);
	test_emit('c');
	mutex_unlock(&m);
	return NULL;
}


static int setup(void) {
	mutex_init(&m);
	test_assert_zero(thread_create(&second, THREAD_FLAG_SUSPENDED, second_run, NULL));
	test_assert_zero(thread_create(&first, THREAD_FLAG_SUSPENDED, first_run, NULL));

	return 0;
}
