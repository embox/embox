/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */

#ifndef RISCV_ASM_ASM_H_
#define RISCV_ASM_ASM_H_

#if __riscv_xlen == 64
#define REG_S sd
#define REG_L ld
#define SZREG 8
#define LGREG 3
#elif __riscv_xlen == 32
#define REG_S sw
#define REG_L lw
#define SZREG 4
#define LGREG 2
#else
#error "Unexpected __riscv_xlen"
#endif

#ifdef __riscv_float_abi_double
#define REG_FS fsd
#define REG_FL fld
#else
#define REG_FS fsw
#define REG_FL flw
#endif

#endif /* RISCV_ASM_ASM_H_ */
