/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#ifndef RISCV_LIB_GDBSTUB_H_
#define RISCV_LIB_GDBSTUB_H_

#include <debug/breakpoint.h>

#define GDB_PACKET_SIZE (sizeof(long) * 33 * 2)

#define GDB_FEATURE_STR "l<target><architecture>riscv</architecture></target>"

#define GDB_BPT_CTX_PC(ctx) (void *)(((struct bpt_context *)(ctx))->excpt.epc)

#define GDB_BPT_CTX_SP(ctx) (void *)(((struct bpt_context *)(ctx))->excpt.sp)

#define GDB_IRQ_CTX_PC(ctx) GDB_BPT_CTX_PC(ctx)

#endif /* RISCV_LIB_GDBSTUB_H_ */
