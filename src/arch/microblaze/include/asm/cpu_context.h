/**
 * @file
 * @brief Defines Microblaze CPU context structure and offsets
 *
 * @date 26.01.09
 * @author Eldar Abusalimov
 */

#ifndef MICROBLAZE_CPU_CONTEXT_H_
#define MICROBLAZE_CPU_CONTEXT_H_

#ifndef __ASSEMBLER__

#define MICROBLAZE_REGISTERS_QUANTITY 0x10
/**
 * Structure holding the whole context of the CPU
 */
typedef struct _CPU_CONTEXT {
	unsigned int regs[MICROBLAZE_REGISTERS_QUANTITY];
	unsigned int pcs[2];
}__attribute__ ((aligned (8))) CPU_CONTEXT;

struct cpu_context {
	uint32_t	r1; /* stack pointer */
	uint32_t	r2;
	/* dedicated registers */
	uint32_t	r13;
	uint32_t	r14;
	uint32_t	r15;
	uint32_t	r16;
	uint32_t	r17;
	uint32_t	r18;
	/* non-volatile registers */
	uint32_t	r19;
	uint32_t	r20;
	uint32_t	r21;
	uint32_t	r22;
	uint32_t	r23;
	uint32_t	r24;
	uint32_t	r25;
	uint32_t	r26;
	uint32_t	r27;
	uint32_t	r28;
	uint32_t	r29;
	uint32_t	r30;
	/* r31 is used as current task pointer */
	/* special purpose registers */
	uint32_t	msr;
	uint32_t	ear;
	uint32_t	esr;
	uint32_t	fsr;
};

#else /* __ASSEMBLER__ */

#endif /* __ASSEMBLER__ */

#endif /* MICROBLAZE_CPU_CONTEXT_H_ */
