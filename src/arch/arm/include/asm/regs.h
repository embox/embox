/**
 * @file
 * @brief Defines register convention for arm architecture
 *
 * @date 19.09.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef ARM_REGS_H_
#define ARM_REGS_H_

#ifdef __ASSEMBLER__

#define pc      r15

#define lr      r14

#define sp      r13

#endif /* __ASSEMBLER__ */

#ifndef __ASSEMBLER__
static inline void *cpu_get_stack(void) {
	void * ret;
	__asm__ __volatile__ (
		"mov %[res],SP"
		: [res]"=r"(ret)
	);
	return ret;
}
#endif

#endif /* ARM_REGS_H_ */
