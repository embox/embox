/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.08.23
 */

#include <stdbool.h>
#include <stddef.h>

#include <debug/breakpoint.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("tests for hardware breakpoints");

static __attribute__((noinline)) void breakpoint_trigger(void) {
	test_emit('a');
}

static void breakpoint_handler(struct bpt_context *ctx) {
	bpt_disable_all();
	{
		breakpoint_trigger();
		bpt_remove(BPT_TYPE_HARD, breakpoint_trigger);
	}
	bpt_enable_all();

	test_emit('b');
}

TEST_CASE("set/remove hardware breakpoints") {
	struct bpt_env new;
	struct bpt_env old;
	size_t count;
	int i;

	bpt_env_save(&old);

	bpt_env_init(&new, breakpoint_handler, true);
	bpt_env_restore(&new);

	count = bpt_get_count(BPT_TYPE_HARD);

	for (i = 0; i < count; i++) {
		test_assert(bpt_set(BPT_TYPE_HARD, breakpoint_handler));
	}

	test_assert(!bpt_set(BPT_TYPE_HARD, breakpoint_trigger));

	bpt_remove_all();

	test_assert(bpt_set(BPT_TYPE_HARD, breakpoint_trigger));

	breakpoint_trigger();

	test_assert_emitted("aba");

	bpt_env_restore(&old);
}
