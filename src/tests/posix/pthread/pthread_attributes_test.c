/**
 * @date    27.05.2023
 * @author  Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <embox/test.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_stack.h>
#include <pthread.h>

EMBOX_TEST_SUITE("posix/pthread attributes");

static void *writer_thread(void *arg) {
	pthread_mutex_t *mutex = (pthread_mutex_t *) arg;
	test_assert_zero(pthread_mutex_lock(mutex));
	test_assert_zero(pthread_mutex_unlock(mutex));

	return NULL;
}

TEST_CASE("Test pthread_getattr_np") {
	pthread_t thread;
	pthread_attr_t attr;
	pthread_mutex_t mutex;

	pthread_attr_init(&attr);
	pthread_mutex_init(&mutex, NULL);

	test_assert_zero(pthread_mutex_lock(&mutex));
	test_assert_zero(pthread_create(&thread, NULL, writer_thread, &mutex));

	test_assert_zero(pthread_getattr_np(thread, &attr));
	test_assert_equal(attr.stack_size, (uint32_t) thread_stack_get_size(thread));
	test_assert_equal(attr.stack, thread_stack_get(thread));

	test_assert_zero(pthread_mutex_unlock(&mutex));

	pthread_mutex_destroy(&mutex);
	pthread_attr_destroy(&attr);
}
