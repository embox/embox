/**
 * \file cpu_context.h
 *
 * \brief Defines CPU context structure and offsets
 *	TODO comments = shit
 * \date Jan 26, 2009
 * \author Eldar Abusalimov
 */

#ifndef CPU_CONTEXT_H_
#define CPU_CONTEXT_H_


#ifndef __ASSEMBLER__
#define MICROBLAZE_REGISTERS_QUANTITY 0x10
/*
 * Structure holding the whole context of the CPU
 */
typedef struct _CPU_CONTEXT {
	unsigned int regs[MICROBLAZE_REGISTERS_QUANTITY];
	unsigned int pcs[2];
}__attribute__ ((aligned (8))) CPU_CONTEXT;

#else /* __ASSEMBLER__ */

#endif /* __ASSEMBLER__ */
#endif /* CPU_CONTEXT_H_ */
