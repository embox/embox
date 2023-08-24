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
	size_t regsize;

	if (regnum == 0) {
		memset(regval, 0, sizeof(long));
		regsize = sizeof(long);
	}
	else if (regnum < 32) {
		memcpy(regval, &ctx->r[regnum - 1], sizeof(long));
		regsize = sizeof(long);
	}
	else if (regnum == 32) {
		memcpy(regval, &ctx->pc, sizeof(long));
		regsize = sizeof(long);
	}
	else {
		regsize = 0;
	}

	return regsize;
}
