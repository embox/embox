/**
 * @file
 * @brief Context init for E2K
 *
 * @date 18.03.2019
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <hal/context.h>
#include <util/binalign.h>
#include <util/log.h>

#include <e2k_api.h>

/* This value is used for both stack base and size align. */
#define E2K_STACK_ALIGN (1UL << 12)

#define round_down(x, bound) ((x) & ~((bound) - 1))

/* Reserve 1/4 for PSP stack, 1/4 for PCSP stack, and 1/2 for USD stack */
#define PSP_CALC_STACK_BASE(sp, size) (sp - (size) / 4)
#define PSP_CALC_STACK_SIZE(sp, size) ((size) / 4)

#define PCSP_CALC_STACK_BASE(sp, size) (sp - (size) / 2)
#define PCSP_CALC_STACK_SIZE(sp, size) ((size) / 4)

#define USD_CALC_STACK_BASE(sp, size) PCSP_CALC_STACK_BASE(sp, size)
#define USD_CALC_STACK_SIZE(sp, size) ((size) / 2)

static void e2k_calculate_stacks(struct context *ctx, uint64_t sp,
	uint64_t size) {
	uint64_t psp_size, pcsp_size, usd_size;
	uint64_t aligned_sp;

	log_debug("Stacks:\n");

	/* Round SP */
	aligned_sp = round_down(sp, E2K_STACK_ALIGN);
	/* Round size. Align size to (4 * E2K_STACK_ALIGN), so
	 * size / 4 will be aligned to E2K_STACK_ALIGN. */
	size = round_down(size - (sp - aligned_sp), 4 * E2K_STACK_ALIGN);
	sp = aligned_sp;

	ctx->psp_lo |= PSP_CALC_STACK_BASE(sp, size) << PSP_BASE;
	ctx->psp_lo |= E2_RWAR_RW_ENABLE << PSP_RW;
	psp_size = PSP_CALC_STACK_SIZE(sp, size);
	assert(psp_size);
	ctx->psp_hi |= psp_size << PSP_SIZE;
	log_debug("  PSP.base=0x%lx, PSP.size=0x%lx\n",
		PSP_CALC_STACK_BASE(sp, size), psp_size);

	ctx->pcsp_lo |= PCSP_CALC_STACK_BASE(sp, size) << PCSP_BASE;
	ctx->pcsp_lo |= E2_RWAR_RW_ENABLE << PCSP_RW;
	pcsp_size = PCSP_CALC_STACK_SIZE(sp, size);
	assert(pcsp_size);
	ctx->pcsp_hi |= pcsp_size << PCSP_SIZE;
	log_debug("  PCSP.base=0x%lx, PCSP.size=0x%lx\n",
		PCSP_CALC_STACK_BASE(sp, size), pcsp_size);

	ctx->usd_lo |= USD_CALC_STACK_BASE(sp, size) << USD_BASE;
	usd_size = USD_CALC_STACK_SIZE(sp, size);
	assert(usd_size);
	ctx->usd_hi |= usd_size << USD_SIZE;
	log_debug("  USD.base=0x%lx, USD.size=0x%lx\n",
		USD_CALC_STACK_BASE(sp, size), usd_size);
}

static void e2k_calculate_crs(struct context *ctx, uint64_t routine_addr) {
	uint64_t usd_size = (ctx->usd_hi >> USD_SIZE) & USD_SIZE_MASK;

	/* Reserve space in hardware stacks for @routine_addr */
	/* Remark: We do not update psp.hi to reserve space for arguments,
	 * since routine do not accepts any arguments. */
	ctx->pcsp_hi |= SZ_OF_CR0_CR1 << PCSP_IND;

	ctx->cr0_hi |= (routine_addr >> CR0_IP) << CR0_IP;

	ctx->cr1_lo |= PSR_ALL_IRQ_ENABLED << CR1_PSR;

	/* Divide on 16 because it field contains size in terms
	 * of 128 bit values. */
	ctx->cr1_hi |= (usd_size >> 4) << CR1_USSZ;
}

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp, unsigned int stack_size) {
	memset(ctx, 0, sizeof(*ctx));

	e2k_calculate_stacks(ctx, (uint64_t)sp, stack_size);

	e2k_calculate_crs(ctx, (uint64_t) routine_fn);

	if (!(flags & CONTEXT_IRQDISABLE)) {
		ctx->upsr |= (UPSR_IE | UPSR_NMIE);
	}
}
