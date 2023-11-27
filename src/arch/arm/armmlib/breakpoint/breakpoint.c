/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#include <assert.h>
#include <stdint.h>

#include <debug/breakpoint.h>

static bpt_handler_t __bpt_handler;

void arm_m_debug_monitor_handler(struct bpt_context *ctx) {
	assert(__bpt_handler);

	__bpt_handler(ctx);
}

void bpt_handler_set(bpt_handler_t handler) {
	__bpt_handler = handler;
}
