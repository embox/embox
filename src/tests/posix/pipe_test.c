/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.11.2013
 */

#include <unistd.h>
#include <pthread.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("pipe() testsuite");

TEST_SETUP(pipe_test_setup);
TEST_TEARDOWN(pipe_test_teardown);

static int pipe_testfd[2];

int pipe_test_setup(void) {

	return pipe(pipe_testfd);
}

int pipe_test_teardown(void) {

	close(pipe_testfd[0]);
	close(pipe_testfd[1]);

	return 0;
}

TEST_CASE("reading small chunks from big data should be successfull") {
	int ret;

	ret = write(pipe_testfd[1], "abcd", 4);
	test_assert(ret != -1);

	for (int i = 0; i < 4; i++) {
		char ch;

		ret = read(pipe_testfd[0], &ch, 1);
		test_assert(1 == ret);

		test_emit(ch);
	}

	test_assert_emitted("abcd");
}

TEST_CASE("reading big chunk from smalls should be successfull") {
	char buf[5];
	int ret;

	for (int i = 0; i < 4; i++) {
		char ch;

		ch = 'a' + i;

		ret = write(pipe_testfd[1], &ch, 1);
		test_assert(1 == ret);
	}

	ret = read(pipe_testfd[0], buf, 4);
	test_assert(ret == 4);

	test_assert_zero(strncmp(buf, "abcd", 4));
}

static void *thd_handler(void *arg) {

	test_assert_zero(usleep(100 * 1000));

	test_emit('b');

	test_assert_equal(4, write(pipe_testfd[1], "abcd", 4));

	return NULL;
}

TEST_CASE("read should block until data avaible") {
	char buf[5];
	pthread_t write_thread;

	test_emit('a');

	test_assert_zero(pthread_create(&write_thread, NULL, thd_handler, NULL));
	test_assert_zero(pthread_detach(write_thread));

	test_assert_equal(4, read(pipe_testfd[0], buf, 4));

	test_assert_zero(usleep(100 * 1000));

	test_emit('c');

	test_assert_zero(strncmp(buf, "abcd", 4));

	test_assert_emitted("abc");
}
