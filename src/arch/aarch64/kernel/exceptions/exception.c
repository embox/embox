/**
 * @brief
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <inttypes.h>
#include <stdint.h>

#include <util/log.h>

#include "exception.h"

void aarch64_print_excpt_context(struct excpt_context *ctx) {
	log_raw(LOG_EMERG,
	    "x0  = %#" PRIx64 "\n"
	    "x1  = %#" PRIx64 "\n"
	    "x2  = %#" PRIx64 "\n"
	    "x3  = %#" PRIx64 "\n"
	    "x4  = %#" PRIx64 "\n"
	    "x5  = %#" PRIx64 "\n"
	    "x6  = %#" PRIx64 "\n"
	    "x7  = %#" PRIx64 "\n"
	    "x8  = %#" PRIx64 "\n"
	    "x9  = %#" PRIx64 "\n"
	    "x10 = %#" PRIx64 "\n"
	    "x11 = %#" PRIx64 "\n"
	    "x12 = %#" PRIx64 "\n"
	    "x13 = %#" PRIx64 "\n"
	    "x14 = %#" PRIx64 "\n"
	    "x15 = %#" PRIx64 "\n"
	    "x16 = %#" PRIx64 "\n"
	    "x17 = %#" PRIx64 "\n"
	    "x18 = %#" PRIx64 "\n"
	    "x19 = %#" PRIx64 "\n"
	    "x20 = %#" PRIx64 "\n"
	    "x21 = %#" PRIx64 "\n"
	    "x22 = %#" PRIx64 "\n"
	    "x23 = %#" PRIx64 "\n"
	    "x24 = %#" PRIx64 "\n"
	    "x25 = %#" PRIx64 "\n"
	    "x26 = %#" PRIx64 "\n"
	    "x27 = %#" PRIx64 "\n"
	    "x28 = %#" PRIx64 "\n"
	    "x29 = %#" PRIx64 "\n"
	    "lr  = %#" PRIx64 "\n"
	    "sp  = %#" PRIx64 "\n"
	    "pc  = %#" PRIx64 "\n"
	    "psr = %#" PRIx32 "\n",
	    ctx->x[0], ctx->x[1], ctx->x[2], ctx->x[3], ctx->x[4], ctx->x[5],
	    ctx->x[6], ctx->x[7], ctx->x[8], ctx->x[9], ctx->x[10], ctx->x[11],
	    ctx->x[12], ctx->x[13], ctx->x[14], ctx->x[15], ctx->x[16], ctx->x[17],
	    ctx->x[18], ctx->x[19], ctx->x[20], ctx->x[21], ctx->x[22], ctx->x[23],
	    ctx->x[24], ctx->x[25], ctx->x[26], ctx->x[27], ctx->x[28], ctx->x[29],
	    ctx->lr, ctx->sp, ctx->pc, (uint32_t)ctx->psr);
}
