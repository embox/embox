/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <debug/breakpoint.h>

size_t gdb_read_reg(struct bpt_context *ctx, int regnum, void *regval) {
	switch (regnum) {
	case 1:
		*(unsigned long *)regval = ctx->excpt.ra;
		break;

	case 2:
		*(unsigned long *)regval = ctx->excpt.sp;
		break;

	case 32:
		*(unsigned long *)regval = ctx->excpt.epc;
		break;

	default:
		return 0;
	}

	return sizeof(unsigned long);
}
