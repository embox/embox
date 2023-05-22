/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.05.23
 */
#ifndef X86_KERNEL_GDB_GDB_H_
#define X86_KERNEL_GDB_GDB_H_

#include <stddef.h>
#include <stdint.h>

#define PACKET_MAX_SIZE 0x80 // must be hex

#define GEN_REGS_SIZE (10 * 4)

#define FEATURE_STR "l<target><architecture>i386</architecture></target>"

#endif /* X86_KERNEL_GDB_GDB_H_ */
