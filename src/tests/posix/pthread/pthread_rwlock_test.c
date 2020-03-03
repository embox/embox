/**
 * @file
 *
 * @date Mar 02, 2020
 * @author: Puranjay Mohan
 */

#include <pthread.h>
#include <poll.h>
#include <embox/test.h>
#include <stdlib.h>
#define ACCESS_ONCE(x) (*(volatile typeof(x) *) &(x))

EMBOX_TEST_SUITE("posix/pthread_rwlock_api");

struct data {
	pthread_rwlock_t lock_rw;
	int x;
	int reader_x;
};

TEST_CASE ("Initialize rwlock with pthread_rwlock_init()") {
	pthread_rwlock_t rw;
	test_assert_zero(pthread_rwlock_init(&rw, NULL));
	test_assert_zero(pthread_rwlock_rdlock(&rw));
	test_assert_zero(pthread_rwlock_unlock(&rw));
	test_assert_zero(pthread_rwlock_wrlock(&rw));
	test_assert_zero(pthread_rwlock_unlock(&rw));
	test_assert_zero(pthread_rwlock_destroy(&rw));
}

static void *reader_thread(void *arg) {
	int i;
	int newx = -1, oldx = -1;

	struct data *lockd = (struct data *)arg;
	pthread_rwlock_t *p = (pthread_rwlock_t *) (&(lockd->lock_rw));

	for (i = 0; i < 100; i++) {
		test_assert_zero(pthread_rwlock_rdlock(p));
		newx = ACCESS_ONCE(lockd->x);
		test_assert_zero(pthread_rwlock_unlock(p));
		poll(NULL, 0, 1);
		if (newx != oldx) {
			lockd->reader_x = newx;
		}
		oldx = newx;
	}
	return NULL;
}

static void *writer_thread(void *arg) {
	int i;
	struct data *lockd = (struct data *) arg;
	pthread_rwlock_t *p = (pthread_rwlock_t *) (&(lockd->lock_rw));

	for (i = 0; i < 3; i++) {
		test_assert_zero(pthread_rwlock_wrlock(p));
		ACCESS_ONCE(lockd->x)++;
		test_assert_zero(pthread_rwlock_unlock(p));
		poll(NULL, 0, 5);
	}
	return NULL;

}

TEST_CASE("Testing Reader Writer Lock") {
	pthread_t tid1, tid2;
	void *vp;
	struct data *lock_data = (struct data *) malloc(sizeof(struct data));

	pthread_rwlock_init(&(lock_data->lock_rw), NULL);
	lock_data->x = 0;
	lock_data->reader_x = 0;

	test_assert_zero(pthread_create(&tid1, NULL, writer_thread, lock_data));
	test_assert_zero(pthread_create(&tid2, NULL, reader_thread, lock_data));
	test_assert_zero(pthread_join(tid1, &vp));
	test_assert_zero(pthread_join(tid2, &vp));
	test_assert_equal(lock_data->x, lock_data->reader_x);
	free(lock_data);
}
