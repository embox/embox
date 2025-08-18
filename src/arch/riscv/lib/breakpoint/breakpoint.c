/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */

#include <assert.h>
#include <stdint.h>

#include <asm/csr.h>
#include <debug/breakpoint.h>
#include <riscv/exception.h>

static bpt_handler_t __bpt_handler;

static void riscv_bpt_excpt_handler(struct excpt_context *ctx) {
	assert(__bpt_handler);

	__bpt_handler((struct bpt_context *)ctx);
}

void bpt_handler_set(bpt_handler_t handler) {
	__bpt_handler = handler;

	if (handler) {
		riscv_excpt_table[RISCV_EXC_BREAKPOINT] = riscv_bpt_excpt_handler;
	}
	else {
		riscv_excpt_table[RISCV_EXC_BREAKPOINT] = NULL;
	}
}
