/**
 * @file
 * @brief Tests thread suspend/resume methods.
 *
 * @date Apr 17, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <stdbool.h>

#include <kernel/thread/api.h>
#include <hal/ipl.h>

// TODO define them in API. -- Eldar
#define THREAD_PRIORITY_LOW  191
#define THREAD_PRIORITY_HIGH 63

EMBOX_TEST_SUITE("Thread suspend/resume");

enum state {
	s_init = 1, s_low, s_high,
};

enum action {
	a_begin, a_middle, a_end,
};

#define index(state) ((state) - 1)

static enum state trans[3][3] = {
	[index(s_init)] = {
		[a_begin] = s_low,
		[a_middle] = s_high,
		[a_end] = s_init,
	},
	[index(s_low)] = {
		[a_begin] = s_high,
		[a_end] = s_init,
	},
	[index(s_high)] = {
		[a_begin] = s_low,
		[a_end] = s_init,
	},
};

static enum state current_state;

static void do_trans_or_fail(enum state s, enum action a);
static void do_trans_or_exit(enum state s, enum action a);

static void *low_run(void *);
static void *high_run(void *);

TEST_CASE() {
	struct thread *low = NULL, *high = NULL;
	void *ret;

	current_state = s_init;

	test_assert_zero(
			thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_not_null(high);
	test_assert_zero(thread_set_priority(high, THREAD_PRIORITY_HIGH));

	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, high));
	test_assert_not_null(low);
	test_assert_zero(thread_set_priority(low, THREAD_PRIORITY_LOW));

	do_trans_or_fail(s_init, a_begin);

	test_assert_zero(thread_resume(low));
	test_assert_zero(thread_join(low, &ret));
	test_assert_null(ret);

	do_trans_or_fail(s_init, a_middle);

	test_assert_zero(thread_resume(high));
	test_assert_zero(thread_join(high, &ret));
	test_assert_null(ret);

	do_trans_or_fail(s_init, a_end);
}

static void *low_run(void *arg) {
	struct thread *high = (struct thread *) arg;

	do_trans_or_exit(s_low, a_begin);

	thread_resume(high);

	do_trans_or_exit(s_low, a_end);

	return NULL;
}

static void *high_run(void *arg) {
	do_trans_or_exit(s_high, a_begin);

	thread_suspend(thread_self());

	do_trans_or_exit(s_high, a_end);

	return NULL;
}

static bool do_trans(enum state s, enum action a) {
	int ret = true;
	ipl_t ipl = ipl_save();

	if (current_state != s) {
		ret = false;
		goto out_restore;
	}

	s = trans[index(s)][a];
	if (!s) {
		ret = false;
		goto out_restore;
	}

	current_state = s;

out_restore:
	ipl_restore(ipl);
	return ret;
}

static void do_trans_or_fail(enum state s, enum action a) {
	test_assert(do_trans(s, a));
}

/* We can't fail() inside another thread. */
static void do_trans_or_exit(enum state s, enum action a) {
	if (!do_trans(s, a)) {
		thread_exit((void *) -1);
	}
}

