/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    09.07.2015
 */

.global hardware_init_hook
hardware_init_hook:
    mov r0, #0
    ldr r1, =arm_trap_table
    /* 16 words */
    ldmia r1!, {r2-r9}
    stmia r0!, {r2-r9}
    ldmia r1!, {r2-r9}
    stmia r0!, {r2-r9}

    /* exit */
    mov pc, lr
