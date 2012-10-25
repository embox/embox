/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

	.text
	.global context_switch
context_switch:

    /* Storing and restoring not user, but current set of registers */

    STMIA   R0!, {R0 - R14}
    MRS     R2, SPSR
    MRS     R3, CPSR
    STMIA   R0, {R2, R3, lr}
    ADD     R1, R1, #60
    LDM     R1, {R2, R3, lr}
    MSR     SPSR_cxsf, R2
    MSR     CPSR, R3
    LDMDB   R1, {R0 - R14}
    NOP
    mov	    pc, lr

