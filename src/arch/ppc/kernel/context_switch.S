/**
 * @file
 *
 * @date 16.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <hal/context.h>

C_ENTRY(context_switch)
	isync
	stmw   r13, CTX_GPR13(r3)
	mflr   r28
	mfcr   r29
	mr     r30, r1
	mr     r31, r2
	stmw   r28, CTX_LR(r3)

	lmw    r28, CTX_LR(r4)
	mr     r2, r31
	mr     r1, r30
	mtcr   r29
	mtlr   r28
	lmw    r13, CTX_GPR13(r4)
	isync

	blr
