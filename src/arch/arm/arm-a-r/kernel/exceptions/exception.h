/**
 * @file
 * @brief
 *
 * @date 06.11.22
 * @author Aleksey Zhmulin
 */
#ifndef ARCH_ARM_ARMLIB_EXCEPTION_H_
#define ARCH_ARM_ARMLIB_EXCEPTION_H_

#include <arm/fpu.h>
#include <asm/ptrace.h>
#include <hal/test/traps_core.h>

#define DEBUG_FAULT            2
#define SECT_TRANSLATION_FAULT 5
#define PAGE_TRANSLATION_FAULT 7
#define SECT_PERMISSION_FAULT  13
#define PAGE_PERMISSION_FAULT  15

typedef struct excpt_context {
	fpu_context_t fpu_context;
	pt_regs_t ptregs;
} excpt_context_t;

extern fault_handler_t arm_data_fault_table[0x10];
extern fault_handler_t arm_inst_fault_table[0x10];

extern void arm_show_excpt_context(excpt_context_t *ctx);

#endif /* ARCH_ARM_ARMLIB_EXCEPTION_H_ */
