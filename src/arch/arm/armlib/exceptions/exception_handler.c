/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2013
 */
#include <util/log.h>

#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/thread.h>

#include <asm/cp15.h>
#include <arm/fpu.h>

uint32_t GLOBAL_LR1 = 0;
uint32_t GLOBAL_SP1 = 0;
uint32_t GLOBAL_CPSR1 = 0;
uint32_t GLOBAL_SPSR1 = 0;

uint32_t GLOBAL_LR2 = 0;
uint32_t GLOBAL_SP2 = 0;
uint32_t GLOBAL_CPSR2 = 0;
uint32_t GLOBAL_SPSR2 = 0;

void mon_swi_handler(void) {
	while (1)
		;
}

void mon_func(void) {
}

#define ARM_GDB_BAD_INSTRUCTION    0xe1212374 /* bkpt 0x1234 */
void arm_exception_handler(unsigned int *regs) {
	uint32_t val;

	if (regs && ((regs[15] & 0x3) == 0) && /* Avoid unaligned access */
		(*(uint32_t*)(regs[15])) == ARM_GDB_BAD_INSTRUCTION) {
		log_debug("BKPT happened");
		regs[15] += 4;
	} else {
		printk("\nEXCEPTION:\n"
				"r0=%08x r1=%08x r2=%08x r3=%08x\n"
				"r4=%08x r5=%08x r6=%08x r7=%08x\n"
				"r8=%08x r9=%08x r10=%08x r11=%08x\n"
				"r12=%08x r14=%08x\n"
				"cpsr=%08x spsr=%08x\n",
				regs[2], regs[3], regs[4], regs[5],
				regs[6], regs[7], regs[8], regs[9],
				regs[10], regs[11], regs[12], regs[13],
				regs[14], regs[15],
				regs[0], regs[1]);
		printk("REGS* = %p\n", regs);
		val = get_cpsr();
		printk("[SMC] cpsr = %x\n", val);
		printk("[SMC] vbar = %x\n",  cp15_get_vbar());
#ifdef CORTEX_A9
		printk("[SMC] mbvar = %x\n", cp15_get_mvbar());
		cp15_set_scr(0x1);
		printk(">>>>>>>>>>>>>>> NS=1 updated \n");
#endif
		while(1);
	}
}

