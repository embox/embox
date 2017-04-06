/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2013
 */
#include <util/log.h>

#include <kernel/panic.h>

#define ARM_GDB_BAD_INSTRUCTION    0xe1212374 /* bkpt 0x1234 */
void arm_exception_handler(unsigned int *regs) {
	if ((*(uint32_t*)(regs[15])) == ARM_GDB_BAD_INSTRUCTION) {
		log_debug("BKPT happened");
		regs[15] += 4;
	} else {
		panic("EXCEPTION:\n"
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
		while(1);
	}
}
