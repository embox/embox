/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.11.23
 */

#include <inttypes.h>

#include <arm/exception.h>
#include <util/log.h>

void arm_show_excpt_context(excpt_context_t *ctx) {
	log_raw(LOG_EMERG,
	    "r0   = %#" PRIx32 "\n"
	    "r1   = %#" PRIx32 "\n"
	    "r2   = %#" PRIx32 "\n"
	    "r3   = %#" PRIx32 "\n"
	    "r4   = %#" PRIx32 "\n"
	    "r5   = %#" PRIx32 "\n"
	    "r6   = %#" PRIx32 "\n"
	    "r7   = %#" PRIx32 "\n"
	    "r8   = %#" PRIx32 "\n"
	    "r9   = %#" PRIx32 "\n"
	    "r10  = %#" PRIx32 "\n"
	    "r11  = %#" PRIx32 "\n"
	    "r12  = %#" PRIx32 "\n"
	    "sp   = %#" PRIx32 "\n"
	    "lr   = %#" PRIx32 "\n"
	    "pc   = %#" PRIx32 "\n"
	    "cpsr = %#" PRIx32 "\n",
	    ctx->ptregs.r[0], ctx->ptregs.r[1], ctx->ptregs.r[2], ctx->ptregs.r[3],
	    ctx->ptregs.r[4], ctx->ptregs.r[5], ctx->ptregs.r[6], ctx->ptregs.r[7],
	    ctx->ptregs.r[8], ctx->ptregs.r[9], ctx->ptregs.r[10],
	    ctx->ptregs.r[11], ctx->ptregs.r[12], ctx->ptregs.sp, ctx->ptregs.lr,
	    ctx->ptregs.pc, ctx->ptregs.cpsr);
}
