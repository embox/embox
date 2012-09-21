/**
 * @file
 * @brief TODO documentation for softirq_test.c -- Eldar Abusalimov
 *
 * @date Aug 23, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <ctype.h>
#include <stddef.h>
#include <kernel/softirq.h>
#include <util/array.h>

EMBOX_TEST_SUITE("basic softirq tests");

struct test_softirq_tuple {
	unsigned int nr;
	softirq_handler_t handler;
	void *data;
};

static void install_test_softirqs(const struct test_softirq_tuple *handlers,
		size_t size) {
	const struct test_softirq_tuple *t;

	array_foreach_ptr(t, handlers, size) {
		test_assert_zero(softirq_install(t->nr, t->handler, t->data));
	}
}

static void uninstall_test_softirqs(const struct test_softirq_tuple *handlers,
		size_t size) {
	const struct test_softirq_tuple *t;

	array_foreach_ptr(t, handlers, size) {
		test_assert_zero(softirq_install(t->nr, NULL, NULL));
	}
}

static void emitting_softirq(unsigned int softirq_nr, void *data) {
	int code = 'k' + softirq_nr - SOFTIRQ_NR_TEST; /* j/k/l */

	test_emit(code);
	test_emit((int) data);
	test_emit(toupper(code));
}

static void delegating_softirq(unsigned int softirq_nr, void *data) {
	int code = 'k' + softirq_nr - SOFTIRQ_NR_TEST; /* j/k/l */

	test_emit(code);
	test_assert_zero(softirq_raise((unsigned int) data));
	test_emit(toupper(code));
}

TEST_CASE("softirq_raise called outside any hardware ISR should invoke "
		"the handler immediately") {
	test_emit('a');
	test_assert_zero(softirq_install(
			SOFTIRQ_NR_TEST, emitting_softirq, (void *) 'x'));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
	test_emit('c');

	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST, NULL, NULL));

	test_assert_emitted("abkxKc");
}

TEST_CASE("softirq_raise called within softirq_lock/softirq_unlock "
		"must defer a call to the handler") {
	test_emit('a');
	test_assert_zero(softirq_install(
			SOFTIRQ_NR_TEST, emitting_softirq, (void *) 'x'));

	softirq_lock();
	{
		test_emit('b');
		test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
		test_emit('c');
	}
	softirq_unlock();

	test_emit('d');

	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST, NULL, NULL));

	test_assert_emitted("abckxKd");
}

TEST_CASE("softirq with lower priority shouldn't preempt another with "
		"higher priority") {
	const struct test_softirq_tuple layout[] = {
		{ SOFTIRQ_NR_TEST_HI, delegating_softirq, (void *) SOFTIRQ_NR_TEST },
		{ SOFTIRQ_NR_TEST,    delegating_softirq, (void *) SOFTIRQ_NR_TEST_LO },
		{ SOFTIRQ_NR_TEST_LO, emitting_softirq,   (void *) 'x' },
	};

	test_emit('a');
	install_test_softirqs(layout, ARRAY_SIZE(layout));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST_HI));
	test_emit('c');

	uninstall_test_softirqs(layout, ARRAY_SIZE(layout));

	test_assert_emitted("abjJkKlxLc");
}

TEST_CASE("softirq with higher priority should preempt another with "
		"lower priority") {
	const struct test_softirq_tuple layout[] = {
		{ SOFTIRQ_NR_TEST_LO, delegating_softirq, (void *) SOFTIRQ_NR_TEST },
		{ SOFTIRQ_NR_TEST,    delegating_softirq, (void *) SOFTIRQ_NR_TEST_HI },
		{ SOFTIRQ_NR_TEST_HI, emitting_softirq,   (void *) 'x' },
	};

	test_emit('a');
	install_test_softirqs(layout, ARRAY_SIZE(layout));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST_LO));
	test_emit('c');

	uninstall_test_softirqs(layout, ARRAY_SIZE(layout));

	test_assert_emitted("ablkjxJKLc");
}

TEST_CASE("mixed test case for softirq with priorities") {
	const struct test_softirq_tuple layout[] = {
		{ SOFTIRQ_NR_TEST,    delegating_softirq, (void *) SOFTIRQ_NR_TEST_HI },
		{ SOFTIRQ_NR_TEST_HI, delegating_softirq, (void *) SOFTIRQ_NR_TEST_LO },
		{ SOFTIRQ_NR_TEST_LO, emitting_softirq,   (void *) 'x' },
	};

	test_emit('a');
	install_test_softirqs(layout, ARRAY_SIZE(layout));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
	test_emit('c');

	uninstall_test_softirqs(layout, ARRAY_SIZE(layout));

	test_assert_emitted("abkjJKlxLc");
}
