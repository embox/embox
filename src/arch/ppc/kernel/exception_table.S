/**
 * @file
 *
 * @date 08.11.12
 * @author Ilia Vaprol
 */

.section .trap_table.text

/**
 * IVORn    Interrupt type
 * -----------------------
 * IVOR0  - Critical Input
 * IVOR1  - Machine Check
 * IVOR2  - Data Storage
 * IVOR3  - Instruction Storage
 * IVOR4  - External Input
 * IVOR5  - Alignment
 * IVOR6  - Program
 * IVOR7  - Floating Point Unavailable
 * IVOR8  - System call
 * IVOR9  - Auxiliary Processor
 * IVOR10 - Decrementer
 * IVOR11 - Fixed Interval Timer
 * IVOR12 - Watchdog Timer
 * IVOR13 - Data TLB Error
 * IVOR14 - Instruction TLB Error
 * IVOR15 - Debug
 */

#define EXCEPT_HND(n, hnd)  \
	.align 4               ;\
        nop;nop;nop;nop    ; /* FIXME else FIT percived as DEC interrupts */\
	.globl except##n##_hnd ;\
	except##n##_hnd:       ;\
		b hnd

/**
 * Align trap table because all excepts handler must have the same HI bytes
 */
.align 8

EXCEPT_HND(0, except_stub)  /* Critical Input interrupt handler */
EXCEPT_HND(1, except_stub)  /* Machine Check interrupt handler */
EXCEPT_HND(2, except_stub)  /* Data Storage interrupt handler */
EXCEPT_HND(3, except_stub)  /* Instruction Storage interrupt handler */
EXCEPT_HND(4, ext_intr_hnd) /* External Input interrupt handler */
EXCEPT_HND(5, except_stub)  /* Alignment interrupt handler */
EXCEPT_HND(6, except_stub)  /* Program interrupt handler */
EXCEPT_HND(7, except_stub)  /* Floating Point Unavailable interrupt handler */
EXCEPT_HND(8, except_stub)  /* System call interrupt handler */
EXCEPT_HND(9, except_stub)  /* Auxiliary Processor interrupt handler */
EXCEPT_HND(10, decr_hnd)    /* Decrementer interrupt handler */
EXCEPT_HND(11, except_stub) /* Fixed Interval Timer interrupt handler */
EXCEPT_HND(12, except_stub) /* Watchdog Timer interrupt handler */
EXCEPT_HND(13, except_stub) /* Data TLB Error interrupt handler */
EXCEPT_HND(14, except_stub) /* Instruction TLB Error interrupt handler */
EXCEPT_HND(15, except_stub) /* Debug interrupt handler */

/* stubs */
#if 0
sys_rst_hnd:
m_chk_hnd:
dsi_hnd:
isi_hnd:
ext_intr_hnd:
align_hnd:
prog_hnd:
fp_unavail_hnd:
sys_call_hnd:
ap_unavail_hnd:
decr_hnd:
fi_tmr_hnd:
w_tmr_hnd:
dtlb_err_hnd:
itlb_err_hnd:
dbg_hnd:
#endif

except_stub:
die:
	b die
