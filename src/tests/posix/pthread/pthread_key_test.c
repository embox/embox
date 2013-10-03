/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("posix/pthread_key_api");

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;


static void make_key(void) {
	pthread_key_create(&key, NULL);
}


TEST_CASE("create key for thread") {
	void *ptr;

	pthread_once(&key_once, make_key);
	ptr = pthread_getspecific(key);
	test_assert_null(ptr);
	ptr = malloc(OBJECT_SIZE);
	test_assert_null(ptr);
	test_assert_zero(pthread_setspecific(key, ptr));


}
