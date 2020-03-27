/**
 * @file
 * @brief   Test pthread_cancel functions
 * @date    17.03.2020
 * @author  Sobhan Mondal <inro20fdm@gmail.com>
 */

#include <pthread.h>
#include <embox/test.h>
#include <unistd.h>

EMBOX_TEST_SUITE("posix/pthread_cancel");

static pthread_t target_tid;
static int flag = 1;

static void *cancel_self(void *arg) {
	test_assert_zero(pthread_cancel(pthread_self()));
	return NULL;
}

TEST_CASE("Cancel self thread") {
	pthread_t tid;
	void *res;

	test_assert_zero(pthread_create(&tid, NULL, cancel_self, NULL));
	test_assert_zero(pthread_join(tid, &res));
}

static void *immediate_thread(void *arg) {
	return NULL;
}

TEST_CASE("Cancel an already executed thread") {
	pthread_t tid;
	void *res;

	test_assert_zero(pthread_create(&tid, NULL, immediate_thread, NULL));
	sleep(1);
	test_assert_not_zero(pthread_cancel(tid));
	test_assert_zero(pthread_join(tid, &res));
}

static void *main_thread(void *arg) {
	sleep(1);
	test_assert_zero(pthread_cancel(target_tid));
	return NULL;
}

static void *target_thread(void *arg) {
	sleep(2);
	return NULL;
}

TEST_CASE("Cancel other thread") {
	pthread_t tid;
	void *res;

	test_assert_zero(pthread_create(&tid, NULL, main_thread, NULL));
	test_assert_zero(pthread_create(&target_tid, NULL, target_thread, NULL));
	test_assert_zero(pthread_join(tid, NULL));
	test_assert_zero(pthread_join(target_tid, &res));
}

static void *cancel_state_dis_en(void *arg) {
	test_assert_zero(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));
	sleep(2);
	test_assert_zero(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	flag = 0;
	sleep(1);
	flag = 1;
	test_fail("Must not execute : Failed pthread_cancel\n");
	return NULL;
}

static void *cancel_state_dis(void *arg) {
	test_assert_zero(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL));
	sleep(2);
	flag = 0;
	return NULL;
}

TEST_CASE("Cancel thread on cancelability state") {
	pthread_t tid1, tid2;
	void *res;

	test_assert_zero(pthread_create(&tid1, NULL, cancel_state_dis_en, NULL));
	sleep(1);
	test_assert_zero(pthread_cancel(tid1));
	test_assert_zero(pthread_join(tid1, &res));
	test_assert_zero(flag);

	test_assert_zero(pthread_create(&tid2, NULL, cancel_state_dis, NULL));
	sleep(1);
	test_assert_zero(pthread_cancel(tid2));
	test_assert_zero(pthread_join(tid2, &res));
	test_assert_zero(flag);
}

static void cleanup_handler(void *arg)
{
	test_assert_equal((*(int *)arg), 1);
	flag = 0;
}

static void *thread_fun1(void *arg) {
	int handler_arg = 1;
	pthread_cleanup_push(cleanup_handler, &handler_arg);
	pthread_cleanup_pop(0);
	return NULL;
}

static void *thread_fun2(void *arg) {
	int handler_arg = 1;
	pthread_cleanup_push(cleanup_handler, &handler_arg);
	pthread_cleanup_pop(1);
	return NULL;
}

TEST_CASE("Test pthread_cleanup_push and pthread_cleanup_pop") {
	pthread_t tid1, tid2;
	void *res1, *res2;

	flag = 1;
	test_assert_zero(pthread_create(&tid1, NULL, thread_fun1, NULL));
	sleep(2);
	test_assert_equal(flag, 1);
	test_assert_zero(pthread_join(tid1, &res1));
	
	flag = 1;
	test_assert_zero(pthread_create(&tid2, NULL, thread_fun2, NULL));
	sleep(2);
	test_assert_equal(flag, 0);
	test_assert_zero(pthread_join(tid2, &res2));
}
