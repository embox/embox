/**
 * @file
 * @brief TODO documentation for mutex_test.c -- Bulychev Anton
 *
 * @date Apr 18, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/mutex.h>
#include <unistd.h>

static struct thread *first, *second;
static struct mutex m;
static int fl = 0;

EMBOX_TEST_SUITE("Mutex test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_join(first, NULL));
	test_assert_zero(thread_join(second, NULL));
	test_assert_emitted("abc");
}

static void *first_run(void *arg) {
	fl = 1;
	mutex_lock(&m);
	test_emit('a');
	usleep(1000);
	test_emit('b');
	mutex_unlock(&m);
	return NULL;
}

static void *second_run(void *arg) {
	while (!fl) ;
	mutex_lock(&m);
	test_emit('c');
	return NULL;
}


static int setup(void) {
	mutex_init(&m);
	test_assert_zero(thread_create(&second, 0, second_run, NULL));
	test_assert_zero(thread_create(&first, 0, first_run, NULL));

	return 0;
}

