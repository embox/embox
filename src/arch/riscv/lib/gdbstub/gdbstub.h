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

#endif /* RISCV_LIB_GDBSTUB_H_ */
