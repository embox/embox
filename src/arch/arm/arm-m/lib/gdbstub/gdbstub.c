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

	if (regnum < 16) {
		memcpy(regval, &ctx->r[regnum], sizeof(uint32_t));
		regsize = sizeof(uint32_t);
	}
	else if (regnum == 25) {
		memcpy(regval, &ctx->psr, sizeof(uint32_t));
		regsize = sizeof(uint32_t);
	}
	else {
		regsize = 0;
	}

	return regsize;
}
