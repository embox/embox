/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("posix/pthread_key_api");

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void make_key(void) {
	pthread_key_create(&key, NULL);
}

static void *foo_thread1(void *arg) {
	int i;
	static pthread_key_t key1;

	test_assert_zero(pthread_key_create(&key1, NULL));
	test_assert_zero(pthread_setspecific(key1, &key1));
	for (i = 0; i < 20000000; i++);
	test_assert_equal(*((int *) pthread_getspecific(key1)), key1);
	return NULL;
}

static void *foo_thread2(void *arg) {
	int i;
	pthread_key_t key2;

	test_assert_zero(pthread_key_create(&key2, NULL));
	test_assert_zero(pthread_setspecific(key2, &key2));
	for (i = 0; i < 20000000; i++);
	test_assert_equal(*((int *)pthread_getspecific(key2)), key2);
	return NULL;
}

TEST_CASE("create key for thread") {
	pthread_t thread1, thread2;

	test_assert_zero(pthread_create(&thread1, NULL, foo_thread1, NULL));
	test_assert_zero(pthread_create(&thread2, NULL, foo_thread2, NULL));
}

TEST_CASE("create key with PTHREAD_ONCE_INIT") {
	void *ptr;
	pthread_once(&key_once, make_key);
	ptr = pthread_getspecific(key);
	test_assert_null(ptr);
	ptr = "some string";
	test_assert_zero(pthread_setspecific(key, ptr));
	test_assert_str_equal((char *)pthread_getspecific(key), "some string");
}
