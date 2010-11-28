/**
 * @file
 * @brief Defines register convention for arm architecture
 *
 * @date 19.09.2010
 * @author Anton Bondarev
 */

#ifndef ARM_REGS_H_
#define ARM_REGS_H_

#ifdef __ASSEMBLER__

#define pc      r15

#define lr      r14

#define sp      r13

#endif /* __ASSEMBLER__ */

#endif /* ARM_REGS_H_ */
