/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * @date    18.03.2015
 */

#include <kernel/panic.h>
#include <kernel/printk.h>
#include <hal/context.h>

#define NMI        2
#define HARD_FAULT 3

static void nmi_handler(void) {
	panic("\n%s\n", __func__);
}

static void hard_fault(void) {
	panic("\n%s\n", __func__);
}

/* Print exception info and return */
void exc_default_handler(struct exc_saved_base_ctx *ctx, int xpsr) {
	int exp_nr = xpsr & 0xFF;

	printk("\nEXCEPTION (0x%x):\n"
			"r0=%08x r1=%08x r2=%08x r3=%08x\n"
			"r12=%08x lr=%08x pc=%08x xPSR=%08x\n",
			exp_nr,
			ctx->r[0], ctx->r[1], ctx->r[2], ctx->r[3],
			ctx->r[4], ctx->lr, ctx->pc, ctx->psr);

	switch (exp_nr) {
	case NMI:
		nmi_handler();
		break;
	case HARD_FAULT:
		hard_fault();
		break;
	default:
		break;
	}
}
