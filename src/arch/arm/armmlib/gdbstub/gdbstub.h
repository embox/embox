/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#ifndef ARMMLIB_GDBSTUB_H_
#define ARMMLIB_GDBSTUB_H_

#include <stddef.h>

#include <debug/breakpoint.h>

#define GDB_PACKET_SIZE     0x88UL

#define GDB_FEATURE_STR     "l<target><architecture>arm</architecture></target>"

#define GDB_BPT_CTX_PC(ctx) (void *)(((struct bpt_context *)(ctx))->r[15])

#define GDB_BPT_CTX_SP(ctx) (void *)(((struct bpt_context *)(ctx))->r[13])

#define GDB_IRQ_CTX_PC(ctx) NULL /* TODO */

#endif /* ARMMLIB_GDBSTUB_H_ */
