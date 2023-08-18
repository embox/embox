/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#ifndef RISCV_LIB_GDBSTUB_H_
#define RISCV_LIB_GDBSTUB_H_

#include <stdint.h>

#define GDB_PACKET_SIZE (sizeof(long) * 33 * 2)

#define GDB_FEATURE_STR "l<target><architecture>riscv</architecture></target>"

#define GDB_REGS_PC(regs) (void *)((regs)->pc)

#define GDB_REGS_SP(regs) (void *)((regs)->r[1])

#define IRQ_CONTEXT_PC(ctx) NULL

struct gdb_regs {
	unsigned long r[31];
	unsigned long mstatus;
	unsigned long pc;
};

#endif /* RISCV_LIB_GDBSTUB_H_ */
