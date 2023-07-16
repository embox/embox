/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */
#ifndef RISCV_ASM_ASM_H_
#define RISCV_ASM_ASM_H_

#ifdef __ASSEMBLER__

#if __riscv_xlen == 64

#define PTR_S sd
#define PTR_L ld
#define SZREG 8
#define LGREG 3

#elif __riscv_xlen == 32

#define PTR_S sw
#define PTR_L lw
#define SZREG 4
#define LGREG 2

#else
#error "Unexpected __riscv_xlen"
#endif

#endif /* __ASSEMBLER__ */

#endif /* RISCV_ASM_ASM_H_ */
