/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#ifndef X86_TRAPS_H_
#define X86_TRAPS_H_

#include <asm/ptrace.h>

#ifndef __ASSEMBLER__

#include <stdint.h>
/*
 * There are 256 IDT entries (each entry is 8 bytes)
 * Vectors 0...31 : system traps and exceptions
 * Vectors 32...237 : device interrupts
 * Vectors 238...255 : special interrupts
 */

/*
 * Hardware-generated exceptions
 */
#define X86_T_DIVIDE_ERROR          0x00 /* Division by zero */
#define X86_T_DEBUG                 0x01 /* Debugger */
#define X86_T_NMI                   0x02 /* Non-maskable interrupt */
#define X86_T_INT3                  0x03 /* Breakpoint */
#define X86_T_OVERFLOW              0x04 /* overflow test */
#define X86_T_OUT_OF_BOUNDS         0x05 /* bounds check */
#define X86_T_INVALID_OPCODE        0x06 /* invalid op code */
#define X86_T_NO_FPU                0x07 /* Coprocessor not available */
#define X86_T_DOUBLE_FAULT          0x08 /* double fault */
#define X86_T_FPU_FAULT             0x09 /* Coprocessor Segment Overrun */
/* BIOS interrupt call for video services 0x10 */
#define X86_T_SEGMENT_NOT_PRESENT   0x0A /* Invalid Task State Segment (TSS) */
#define X86_T_STACK_FAULT           0x0C /* Stack Fault */
#define X86_T_GENERAL_PROTECTION    0x0D /* General Protection */
#define X86_T_PAGE_FAULT            0x0E /* Page Fault */



extern void idt_set_gate(uint8_t nr, uint32_t base, uint16_t sel, uint8_t attr);
extern void idt_init(void);
extern void gdt_init(void);

#else


#endif /* __ASSEMBLER__ */

#endif /* X86_TRAPS_H_ */
