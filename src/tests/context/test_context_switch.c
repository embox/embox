/**
 * @file
 * @brief This file is derived from EMBOX test template.
 */

#include <hal/context.h>
#include <embox/test.h>
#include <stdio.h>

#define STACK_SZ 0x100

EMBOX_TEST(run);

static struct context entry_context, test_context;
static char entry_stack[STACK_SZ];

static void entry(int arg) {
	TRACE("entry begin\n");
	context_switch(&entry_context, &test_context);
	TRACE("entry end (should not be reached)\n");
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

	TRACE("test begin\n");
	context_switch(&test_context, &entry_context);
	TRACE("test end\n");

	return result;
}
