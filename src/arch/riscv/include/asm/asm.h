/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */

#ifndef RISCV_ASM_ASM_H_
#define RISCV_ASM_ASM_H_

#if __riscv_xlen == 32
#define __ASM_SEL_X(xlen32, xlen64) xlen32
#elif __riscv_xlen == 64
#define __ASM_SEL_X(xlen32, xlen64) xlen64
#else
#error "Unexpected __riscv_xlen"
#endif /* __riscv_xlen */

#if !defined(__riscv_flen) || __riscv_flen == 0
#define __ASM_SEL_F(flen0, flen32, flen64) flen0
#elif __riscv_flen == 32
#define __ASM_SEL_F(flen0, flen32, flen64) flen32
#elif __riscv_flen == 64
#define __ASM_SEL_F(flen0, flen32, flen64) flen64
#else
#error "Unexpected __riscv_flen"
#endif /* __riscv_flen */

#define REG_S  __ASM_SEL_X(sw, sd)
#define REG_L  __ASM_SEL_X(lw, ld)
#define REG_SC __ASM_SEL_X(sc.w, sc.d)
#define REG_LR __ASM_SEL_X(lr.w, lr.d)
#define REG_FS __ASM_SEL_F(fsw, fsw, fsd)
#define REG_FL __ASM_SEL_F(flw, flw, fld)

#define REG_SIZE_X __ASM_SEL_X(4, 8)
#define REG_SIZE_F __ASM_SEL_F(0, 4, 8)

#endif /* RISCV_ASM_ASM_H_ */
