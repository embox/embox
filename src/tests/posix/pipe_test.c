/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.11.2013
 */

#include <unistd.h>
#include <kernel/thread.h>

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

	write(pipe_testfd[1], "abcd", 4);

	for (int i = 0; i < 4; i++) {
		char ch;

		test_assert(1 == read(pipe_testfd[0], &ch, 1));

		test_emit(ch);
	}

	test_assert_emitted("abcd");
}

TEST_CASE("reading big chunk from smalls should be successfull") {
	char buf[5];

	for (int i = 0; i < 4; i++) {
		char ch;

		ch = 'a' + i;

		test_assert(1 == write(pipe_testfd[1], &ch, 1));
	}

	read(pipe_testfd[0], buf, 4);

	test_assert_zero(strncmp(buf, "abcd", 4));
}

void *thd_handler(void *arg) {

	usleep(100 * 1000);

	test_emit('b');

	write(pipe_testfd[1], "abcd", 4);

	return NULL;
}

TEST_CASE("read should block until data avaible") {
	char buf[5];

	test_emit('a');

	thread_create(THREAD_FLAG_DETACHED, thd_handler, NULL);

	test_assert_equal(4, read(pipe_testfd[0], buf, 4));

	usleep(100 * 1000);

	test_emit('c');

	test_assert_zero(strncmp(buf, "abcd", 4));

	test_assert_emitted("abc");
}
