/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */
.globl delay
delay:
    subs r0, r0, #1
    bne delay
    bx lr
