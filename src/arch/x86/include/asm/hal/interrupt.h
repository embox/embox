#ifndef HAL_INTERRUPT_H_
# error "Do not include this file directly!"
#endif /* HAL_INTERRUPT_H_ */

#define __INTERRUPT_NRS_TOTAL 15

#ifndef __ASSEMBLER__

typedef unsigned char __interrupt_nr_t;
typedef unsigned long __interrupt_mask_t;

/**
 * There are 256 IDT entries (each entry is 8 bytes)
 * Vectors 0...31 : system traps and exceptions
 * Vectors 32...237 : device interrupts
 * Vectors 238...255 : special interrupts
 */

/**
 * Hardware-generated exceptions
 */
#define T_DIVIDE_ERROR          0x00 /* Division by zero */
#define T_DEBUG                 0x01 /* Debugger */
#define T_NMI                   0x02 /* Non-maskable interrupt */
#define T_INT3                  0x03 /* Breakpoint */
#define T_OVERFLOW              0x04 /* overflow test */
#define T_OUT_OF_BOUNDS         0x05 /* bounds check */
#define T_INVALID_OPCODE        0x06 /* invalid op code */
#define T_NO_FPU                0x07 /* Coprocessor not available */
#define T_DOUBLE_FAULT          0x08 /* double fault */
#define T_FPU_FAULT             0x09 /* Coprocessor Segment Overrun */
/* BIOS interrupt call for video services 0x10 */
#define T_SEGMENT_NOT_PRESENT   0x0A /* Invalid Task State Segment (TSS) */
#define T_STACK_FAULT           0x0C /* Stack Fault */
#define T_GENERAL_PROTECTION    0x0D /* General Protection */
#define T_PAGE_FAULT            0x0E /* Page Fault */

extern void irqc_set_mask(__interrupt_mask_t mask);
extern __interrupt_mask_t irqc_get_mask(void);

#endif /*__ASSEMBLER__*/

