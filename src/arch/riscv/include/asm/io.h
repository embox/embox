/**
 * @file
 *
 * @brief
 *
 * @date 13.03.2020
 * @author Nastya Nizharadze
 */

#ifndef RISCV_IO_H_
#define RISCV_IO_H_

#include <stdint.h>

#define riscv_read32(val,addr) asm volatile("lw %0, 0(%1)" : "=r" (val) : "r" (addr))
#define riscv_write32(val,addr) asm volatile("sw %0, 0(%1)" : : "r" (val), "r" (addr))
#endif /* RISCV_IO_H_ */