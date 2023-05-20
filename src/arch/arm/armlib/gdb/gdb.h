/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#ifndef ARMLIB_GDB_GDB_H_
#define ARMLIB_GDB_GDB_H_

#include <stddef.h>
#include <stdint.h>

#define PACKET_MAX_SIZE 0x80 // must be hex

#define GEN_REGS_SIZE (16 * 4)

#define FEATURE_STR "l<target><architecture>arm</architecture></target>"

#endif /* ARMLIB_GDB_GDB_H_ */
