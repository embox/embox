/**
 * @file
 * @brief Restore shared STDIN flags after a tish command.
 */

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cmd/shell.h>
#include <embox/test.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("tish STDIN flags");

TEST_SETUP(stdin_flags_setup);
TEST_TEARDOWN(stdin_flags_teardown);

static int original_flags;
static int saved_stdin = -1;
static int pipe_fds[2] = {-1, -1};

/* Unit tests run before the console is opened, so the suite brings its own
 * STDIN: a pipe read end placed at STDIN_FILENO. */
static int stdin_flags_setup(void) {
	saved_stdin = dup(STDIN_FILENO);
	if (pipe(pipe_fds) == -1) {
		return -1;
	}
	if (pipe_fds[0] != STDIN_FILENO
	    && dup2(pipe_fds[0], STDIN_FILENO) != STDIN_FILENO) {
		return -1;
	}

	original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (original_flags == -1) {
		return -1;
	}

	return fcntl(STDIN_FILENO, F_SETFL, (original_flags | O_APPEND) & ~O_NONBLOCK);
}

static int stdin_flags_teardown(void) {
	if (pipe_fds[0] != STDIN_FILENO) {
		close(STDIN_FILENO);
	}
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	pipe_fds[0] = pipe_fds[1] = -1;
	if (saved_stdin >= 0) {
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
		saved_stdin = -1;
	}

	return 0;
}

static void *set_stdin_nonblock(void *arg) {
	_exit(fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1);
	return NULL;
}

TEST_CASE("a child changes the shared STDIN flags") {
	pid_t pid;
	int status;
	int flags;

	pid = new_task("", set_stdin_nonblock, NULL);
	test_assert(pid >= 0);
	test_assert_equal(pid, waitpid(pid, &status, 0));
	test_assert(WIFEXITED(status));
	test_assert_zero(WEXITSTATUS(status));
	flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	test_assert(flags != -1);
	test_assert(flags & O_NONBLOCK);
	test_assert_zero(flags & O_APPEND);
}

TEST_CASE("tish restores STDIN flags after a foreground command") {
	const struct shell *sh;
	int flags;

	sh = shell_lookup("tish");
	test_assert_not_null(sh);
	flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	test_assert(flags != -1);
	test_assert(flags & O_APPEND);
	test_assert_zero(flags & O_NONBLOCK);
	test_assert_zero(shell_exec(sh, "tish_set_stdin_nonblock"));
	test_assert_equal(flags, fcntl(STDIN_FILENO, F_GETFL, 0));
}
