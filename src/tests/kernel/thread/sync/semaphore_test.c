/**
 * @file
 * @brief Tests semaphore enter/leave methods.
 *
 *
 * @date Apr 30, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/mutex.h>
#include <unistd.h>

static struct thread *first, *second, *third;
static struct sema s;

EMBOX_TEST_SUITE("Mutex test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_resume(first));
	test_assert_zero(thread_join(first, NULL));
	test_assert_zero(thread_join(second, NULL));
	test_assert_zero(thread_join(third, NULL));
	test_assert_emitted("abcdef");
}

static void *first_run(void *arg) {
	sema_enter(&s);
	test_emit('a');
	test_assert_zero(thread_resume(second));
	usleep(1000);
	test_emit('c');
	sema_leave(&s);
	return NULL;
}

static void *second_run(void *arg) {
	sema_enter(&s);
	test_emit('b');
	test_assert_zero(thread_resume(third));
	usleep(2000);
	sema_leave(&s);
	test_emit('f');
	return NULL;
}

static void *third_run(void *arg) {
	sema_enter(&s);
	test_emit('d');
	sema_leave(&s);
	test_emit('e');
	return NULL;
}

static int setup(void) {
	sema_init(&s, 2);
	test_assert_zero(thread_create(&first, THREAD_FLAG_SUSPENDED, first_run, NULL));
	test_assert_zero(thread_create(&second, THREAD_FLAG_SUSPENDED, second_run, NULL));
	test_assert_zero(thread_create(&third, THREAD_FLAG_SUSPENDED, third_run, NULL));

	return 0;
}
