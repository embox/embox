/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.04.2014
 */

#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

#include <embox/test.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("waitpid testsuite");

#define TEST_WAITPID_CHILD_ECODE 103
#define TEST_WAITPID_SLEEP_MUL   1000

struct test_param {
	int sleep_val;
	int exit_code;
};

void void2param(void *arg, struct test_param *par) {
	par->sleep_val = (unsigned long) arg & 0x0000ffff;
	par->exit_code = (unsigned long) arg >> 16;
}

void *param2void(int sleep_val, int exit_code) {
	return (void *)(uintptr_t)((sleep_val & 0x0000ffff) | (exit_code << 16));
}

static void *test_waitpid_child(void *arg) {
	struct test_param tp;
	void2param(arg, &tp);
	usleep(tp.sleep_val);
	_exit(tp.exit_code);
}

TEST_CASE("waitpid for any child should wait till child finished") {
	pid_t child_pid, wp_ret;
	int status;

	child_pid = new_task("", test_waitpid_child,
			param2void(TEST_WAITPID_SLEEP_MUL,
				TEST_WAITPID_CHILD_ECODE));

	wp_ret = waitpid(-1, &status, 0);
	test_assert_equal(wp_ret, child_pid);
	test_assert(WIFEXITED(status));
	test_assert_equal(WEXITSTATUS(status), TEST_WAITPID_CHILD_ECODE);
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));
}

TEST_CASE("waitpid WNOHANG for any child shouldn't wait till child finished") {
	pid_t child_pid, wp_ret;
	int status;

	child_pid = new_task("", test_waitpid_child,
			param2void(TEST_WAITPID_SLEEP_MUL,
				TEST_WAITPID_CHILD_ECODE));

	wp_ret = waitpid(-1, &status, WNOHANG);
	test_assert_equal(wp_ret, 0);

	wp_ret = waitpid(-1, &status, 0);
	test_assert_equal(wp_ret, child_pid);
	test_assert(WIFEXITED(status));
	test_assert_equal(WEXITSTATUS(status), TEST_WAITPID_CHILD_ECODE);
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));
}

TEST_CASE("waitpid for explicit child should wait till child finished") {
	pid_t child_pid, wp_ret;
	int status;

	child_pid = new_task("", test_waitpid_child,
			param2void(TEST_WAITPID_SLEEP_MUL,
				TEST_WAITPID_CHILD_ECODE));

	wp_ret = waitpid(child_pid, &status, 0);
	test_assert_equal(wp_ret, child_pid);
}

TEST_CASE("waitpid for any child should wait till all child finished") {
	pid_t cpid[2], wp_ret;
	int ecode_sum = 0;
	int pid_sum = 0;
	int status;
	int i;

	for (i = 0; i < 2; i++) {
		cpid[i] = new_task("", test_waitpid_child,
				param2void(TEST_WAITPID_SLEEP_MUL,
					TEST_WAITPID_CHILD_ECODE + i));
		pid_sum += cpid[i];
		ecode_sum += i;
	}

	for (i = 0; i < 2; i++) {
		wp_ret = waitpid(-1, &status, 0);
		pid_sum -= wp_ret;
		test_assert(WIFEXITED(status));
		ecode_sum -= WEXITSTATUS(status) - TEST_WAITPID_CHILD_ECODE;
		test_assert(!WIFSIGNALED(status));
		test_assert(!WIFSTOPPED(status));
		test_assert(!WIFCONTINUED(status));
	}

	test_assert_zero(pid_sum);
	test_assert_zero(ecode_sum);
}

TEST_CASE("waitpid for any child should wait till child finished in order") {
	pid_t cpid[2], wp_ret;
	int status;
	int i;

	for (i = 0; i < 2; i++) {
		cpid[i] = new_task("", test_waitpid_child,
				param2void(i * TEST_WAITPID_SLEEP_MUL,
					TEST_WAITPID_CHILD_ECODE + i));
	}

	for (i = 0; i < 2; i++) {
		wp_ret = waitpid(-1, &status, 0);
		test_assert_equal(cpid[i], wp_ret);
		test_assert(WIFEXITED(status));
		test_assert_equal(TEST_WAITPID_CHILD_ECODE + i, WEXITSTATUS(status));
		test_assert(!WIFSIGNALED(status));
		test_assert(!WIFSTOPPED(status));
		test_assert(!WIFCONTINUED(status));
	}
}

TEST_CASE("waitpid for target child should wait till it finished") {
	pid_t cpid[2], wp_ret;
	int status;
	int i;

	for (i = 0; i < 2; i++) {
		cpid[i] = new_task("", test_waitpid_child,
				param2void(i * TEST_WAITPID_SLEEP_MUL,
					TEST_WAITPID_CHILD_ECODE + i));
	}

	wp_ret = waitpid(cpid[1], &status, 0);
	test_assert_equal(cpid[1], wp_ret);
	test_assert(WIFEXITED(status));
	test_assert_equal(TEST_WAITPID_CHILD_ECODE + 1, WEXITSTATUS(status));
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));

	wp_ret = waitpid(-1, &status, 0);
	test_assert_equal(cpid[0], wp_ret);
	test_assert(WIFEXITED(status));
	test_assert_equal(TEST_WAITPID_CHILD_ECODE, WEXITSTATUS(status));
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));
}

TEST_CASE("waitpid for target child should wait till it finished") {
	pid_t cpid[2], wp_ret;
	int status;
	int i;

	for (i = 0; i < 2; i++) {
		cpid[i] = new_task("", test_waitpid_child,
				param2void(i * TEST_WAITPID_SLEEP_MUL,
					TEST_WAITPID_CHILD_ECODE + i));
	}

	wp_ret = waitpid(cpid[1], &status, 0);
	test_assert_equal(cpid[1], wp_ret);
	test_assert(WIFEXITED(status));
	test_assert_equal(TEST_WAITPID_CHILD_ECODE + 1, WEXITSTATUS(status));
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));

	wp_ret = waitpid(-1, &status, 0);
	test_assert_equal(cpid[0], wp_ret);
	test_assert(WIFEXITED(status));
	test_assert_equal(TEST_WAITPID_CHILD_ECODE, WEXITSTATUS(status));
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));
}

static void *test_waitpid_child_returning(void *arg) {
	usleep(1000);
	return NULL;
}

TEST_CASE("waitpid for any child should wait till child finished via return") {
	pid_t child_pid, wp_ret;
	int status;

	child_pid = new_task("", test_waitpid_child_returning, NULL);

	wp_ret = waitpid(-1, &status, 0);
	test_assert_equal(wp_ret, child_pid);
	test_assert(WIFEXITED(status));
	test_assert_equal(WEXITSTATUS(status), 0);
	test_assert_equal(status, 0);
	test_assert(!WIFSIGNALED(status));
	test_assert(!WIFSTOPPED(status));
	test_assert(!WIFCONTINUED(status));
}
