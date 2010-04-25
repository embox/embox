/**
 * @file
 * @brief This file is derived from EMBOX test template.
 */

#include <hal/context.h>
#include <embox/test.h>
#include <stdio.h>

#define STACK_SZ 0x1000

EMBOX_TEST(run);

static struct context entry_context, infinite_context, redundant_context;
static char entry_stack[STACK_SZ];
static char infinite_stack[STACK_SZ];

static void entry(int arg) {
	TRACE("entry begin\n");
	context_switch(&entry_context, &infinite_context);
	TRACE("entry end (should not be reached)\n");
}

static void infinite (int arg) {
	while (true) {
		TRACE("?");
		context_switch(&infinite_context, &entry_context);
	}
}
/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	context_init(&entry_context, true);
	context_set_entry(&entry_context, entry, 0);
	context_set_stack(&entry_context, entry_stack + STACK_SZ);

	context_init(&infinite_context, true);
	context_set_entry(&infinite_context, infinite, 0);
	context_set_stack(&infinite_context, infinite_stack + STACK_SZ);

	TRACE("test begin\n");
#if 0
	context_switch(&redundant_context, &infinite_context);
#endif
	context_switch(&redundant_context, &entry_context);
	TRACE("test end\n");

	return result;
}
