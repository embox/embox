/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * 	- initial implementation
 * @author  Anton Kozlov
 * @date    17.08.2015
 */

.global copy_excpt_table
copy_excpt_table:
    ldr r1, =arm_trap_table
    cmp r0, r1
    beq 1f
    /* 16 words */
    ldmia r1!, {r2-r9}
    stmia r0!, {r2-r9}
    ldmia r1!, {r2-r9}
    stmia r0!, {r2-r9}
1:
    /* exit */
    mov pc, lr
