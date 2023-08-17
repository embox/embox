/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#ifndef ARMLIB_GDBSTUB_H_
#define ARMLIB_GDBSTUB_H_

#include <stdint.h>

#include <arm/exception.h>

#define GDB_PACKET_SIZE 0x88UL

#define GDB_FEATURE_STR "l<target><architecture>arm</architecture></target>"

#define GDB_REGS_PC(regs) (void *)((regs)->r[15])

#define GDB_REGS_SP(regs) (void *)((regs)->r[13])

#define IRQ_CONTEXT_PC(ctx) (void *)(((excpt_context_t *)(ctx))->ptregs.pc)

struct gdb_regs {
	uint32_t r[16];
	uint32_t psr;
};

#endif /* ARMLIB_GDBSTUB_H_ */
