/**
 * @file
 * @brief TODO documentation for core_test.c -- Alina Kramar
 *
 * @date 01.05.11
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>

EMBOX_TEST_SUITE("Priority inheritance for mutex");

static void *low_run(void *);
static void *mid_run(void *);
static void *high_run(void *);

static struct thread *low;
static struct thread *mid;
static struct thread *high;

TEST_CASE("without inheritance") {
	struct mutex mutex;
	thread_priority_t l = 10, m = 20, h = 30;

	mutex_init(&mutex);

	test_assert_zero(
			thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, (void *) &mutex));
	test_assert_not_null(low);
	test_assert_zero(thread_set_priority(low, l));

	test_assert_zero(
			thread_create(&mid, THREAD_FLAG_SUSPENDED, mid_run, NULL));
	test_assert_not_null(mid);
	test_assert_zero(thread_set_priority(mid, m));

	test_assert_zero(
			thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, (void *) &mutex));
	test_assert_not_null(high);
	test_assert_zero(thread_set_priority(high, h));

	thread_resume(low);

	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));

	test_assert_emitted("abcdefghijk");
//	TRACE("%s", test_get_emitted_into(&buff));
}

static void *low_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit('a');

	mutex_lock(m);

	test_emit('b');

	thread_resume(high);
	test_emit('d');
	thread_resume(mid);

	test_emit('e');

	mutex_unlock(m);

	test_emit('k');

	return NULL;
}

static void *mid_run(void *arg) {
	test_emit('i');
	test_emit('j');

	return NULL;
}

static void *high_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit('c');

	mutex_lock(m);

	test_emit('f');
	test_emit('g');

	mutex_unlock(m);

	test_emit('h');

	return NULL;
}
