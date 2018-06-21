/*
 * e2k_sic.h
 *
 *  Created on: Mar 23, 2018
 *      Author: anton
 */

#ifndef SRC_ARCH_E2K_INCLUDE_E2K_SIC_H_
#define SRC_ARCH_E2K_INCLUDE_E2K_SIC_H_

#include <stdint.h>

#define	E2K_FULL_SIC_BIOS_AREA_PHYS_BASE	0x0000000100000000UL
#define	E2K_FULL_SIC_BIOS_AREA_SIZE		0x0000000001000000UL
#define	E2K_FULL_SIC_CPU_STARTUP_ADDR		E2K_FULL_SIC_BIOS_AREA_PHYS_BASE
#define	E2K_FULL_SIC_IO_AREA_PHYS_BASE		0x0000000101000000UL
#define	E2K_FULL_SIC_IO_AREA_SIZE		0x0000000000010000UL /* 64K */

#define	E2K_LEGACY_SIC_BIOS_AREA_PHYS_BASE	0x000000fff0000000UL
#define	E2K_LEGACY_SIC_BIOS_AREA_SIZE		0x0000000010000000UL /* 256M */
#define	E2K_LEGACY_SIC_CPU_STARTUP_ADDR		\
		E2K_LEGACY_SIC_BIOS_AREA_PHYS_BASE
#define	E2K_LEGACY_SIC_IO_AREA_PHYS_BASE	0x000000ff20000000UL
#define	E2K_LEGACY_SIC_IO_AREA_SIZE		0x0000000010000000UL /* 256M */


#define E3S_NSR_AREA_PHYS_BASE		0x0000000200000000UL	/* node 0 */
#define E3S_NSR_AREA_SIZE		0x0000000020000000UL	/* size of 1 */
								/* node SR */
								/* area */
#define	E3S_PCICFG_AREA_OFFSET		0x0000000000000000UL	/* offset of */
								/* PCI CFG */
								/* area at */
								/* NSR */
#define	E3S_PCICFG_AREA_SIZE		0x0000000010000000UL	/* size of */
								/* PCI CFG */
								/* area */
#define	E3S_SBSR_AREA_OFFSET		0x0000000010000000UL	/* offset of */
								/* SB (IOHUB) */
								/* registers */
								/* area */
#define	E3S_SBSR_AREA_SIZE		0x0000000000010000UL	/* size of */
								/* IOHUB */
								/* registers */
								/* area */
#define	E3S_NBSR_AREA_OFFSET		0x0000000014000000UL	/* offset of */
								/* NB regs */
								/* into NSR */
								/* area */
#define	E3S_NBSR_AREA_SIZE		0x0000000000010000UL	/* size of */
								/* NBSR area */
								/* of 1 node */
#define ES2_NSR_AREA_PHYS_BASE		0x0000000110000000UL	/* node 0 */
#define ES2_NSR_AREA_MAX_SIZE		0x0000000010000000UL	/* max NSRs */
								/* area: */
								/* limited */
								/* by follow */
								/* LAPICINT */
#define	ES2_NBSR_AREA_OFFSET		0x0000000000000000UL	/* offset of */
								/* NB regs */
								/* into NSR */
								/* area */
#define	ES2_NBSR_AREA_SIZE		0x0000000000100000UL	/* size of */
								/* NBSR area */
								/* of 1 node */
#define ES2_COPSR_AREA_PHYS_BASE	0x00000001c0000000UL	/* Co-Proc */
								/* area start */
								/* DSP 0 of */
								/* node 0 */
#define	ES2_COPSR_AREA_SIZE		0x0000000001000000UL	/* Co-Proc */
								/* one node */
								/* area size */
#define	ES2_PCICFG_AREA_PHYS_BASE	0x0000000200000000UL	/* start of */
								/* PCI CFG */
								/* area */
#define	ES2_PCICFG_AREA_SIZE		0x0000000010000000UL	/* size of */
								/* PCI CFG */
								/* area of */
								/* 1 IO link */
#define	E1CP_PCICFG_AREA_PHYS_BASE	0x000000ff10000000UL	/* start of */
								/* PCI CFG */
								/* area */
#define	E1CP_PCICFG_AREA_SIZE		0x0000000010000000UL	/* 256M */
								/* size of */
								/* PCI CFG */
								/* area of */
								/* 1 IO link */
/* on e2s same as on es2 */
#define E2S_NSR_AREA_PHYS_BASE		ES2_NSR_AREA_PHYS_BASE
#define E2S_NSR_AREA_MAX_SIZE		ES2_NSR_AREA_MAX_SIZE
#define	E2S_NBSR_AREA_OFFSET		ES2_NBSR_AREA_OFFSET
#define	E2S_NBSR_AREA_SIZE		ES2_NBSR_AREA_SIZE
#define E2S_COPSR_AREA_PHYS_BASE	ES2_COPSR_AREA_PHYS_BASE
#define	E2S_COPSR_AREA_SIZE		ES2_COPSR_AREA_SIZE
#define	E2S_PCICFG_AREA_PHYS_BASE	ES2_PCICFG_AREA_PHYS_BASE
#define	E2S_PCICFG_AREA_SIZE		ES2_PCICFG_AREA_SIZE
/* on e8c same as on es2 & e2s */
#define E8C_NSR_AREA_PHYS_BASE		ES2_NSR_AREA_PHYS_BASE
#define E8C_NSR_AREA_MAX_SIZE		ES2_NSR_AREA_MAX_SIZE
#define	E8C_NBSR_AREA_OFFSET		ES2_NBSR_AREA_OFFSET
#define	E8C_NBSR_AREA_SIZE		ES2_NBSR_AREA_SIZE
#define E8C_COPSR_AREA_PHYS_BASE	ES2_COPSR_AREA_PHYS_BASE
#define	E8C_COPSR_AREA_SIZE		ES2_COPSR_AREA_SIZE
#define	E8C_PCICFG_AREA_PHYS_BASE	ES2_PCICFG_AREA_PHYS_BASE
#define	E8C_PCICFG_AREA_SIZE		ES2_PCICFG_AREA_SIZE
/* on e8c2 same as on es2 & e2s & e8c */
#define E8C2_NSR_AREA_PHYS_BASE		ES2_NSR_AREA_PHYS_BASE
#define E8C2_NSR_AREA_MAX_SIZE		ES2_NSR_AREA_MAX_SIZE
#define	E8C2_NBSR_AREA_OFFSET		ES2_NBSR_AREA_OFFSET
#define	E8C2_NBSR_AREA_SIZE		ES2_NBSR_AREA_SIZE
#define E8C2_COPSR_AREA_PHYS_BASE	ES2_COPSR_AREA_PHYS_BASE
#define	E8C2_COPSR_AREA_SIZE		ES2_COPSR_AREA_SIZE
#define	E8C2_PCICFG_AREA_PHYS_BASE	ES2_PCICFG_AREA_PHYS_BASE
#define	E8C2_PCICFG_AREA_SIZE		ES2_PCICFG_AREA_SIZE
/* on e1c+ legacy e2s SIC is used */
#define E1CP_NSR_AREA_PHYS_BASE		early_get_legacy_nbsr_base()
#define E1CP_NSR_AREA_MAX_SIZE		E2S_NSR_AREA_MAX_SIZE
#define	E1CP_NBSR_AREA_OFFSET		E2S_NBSR_AREA_OFFSET
#define	E1CP_NBSR_AREA_SIZE		E2S_NBSR_AREA_SIZE

/*
 * NBR area configuration
 */

#define	E2K_NSR_AREA_PHYS_BASE			\
({						\
	uint64_t ret = -1;				\
						\
	if (IS_MACHINE_E3S)			\
		ret = E3S_NSR_AREA_PHYS_BASE;	\
	else if (IS_MACHINE_ES2)		\
		ret = ES2_NSR_AREA_PHYS_BASE;	\
	else if (IS_MACHINE_E2S)		\
		ret = E2S_NSR_AREA_PHYS_BASE;	\
	else if (IS_MACHINE_E8C)		\
		ret = E8C_NSR_AREA_PHYS_BASE;	\
	else if (IS_MACHINE_E1CP)		\
		ret = E1CP_NSR_AREA_PHYS_BASE;	\
	else if (IS_MACHINE_E8C2)		\
		ret = E8C2_NSR_AREA_PHYS_BASE;	\
	else					\
		assert(0);				\
						\
	ret;					\
})

#define	E2K_NSR_AREA_SIZE			\
({						\
	uint64_t ret = -1;				\
						\
	if (IS_MACHINE_E3S)			\
		ret = E3S_NSR_AREA_SIZE;	\
	else if (IS_MACHINE_ES2)		\
		ret = ES2_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E2S)		\
		ret = E2S_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C)		\
		ret = E8C_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E1CP)		\
		ret = E1CP_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C2)		\
		ret = E8C2_NBSR_AREA_SIZE;	\
	else					\
		assert(0);				\
						\
	ret;					\
})

#define	E2K_NBSR_OFFSET				\
({						\
	uint64_t ret = -1;				\
						\
	if (IS_MACHINE_E3S)			\
		ret = E3S_NBSR_AREA_OFFSET;	\
	else if (IS_MACHINE_ES2)		\
		ret = ES2_NBSR_AREA_OFFSET;	\
	else if (IS_MACHINE_E2S)		\
		ret = E2S_NBSR_AREA_OFFSET;	\
	else if (IS_MACHINE_E8C)		\
		ret = E8C_NBSR_AREA_OFFSET;	\
	else if (IS_MACHINE_E1CP)		\
		ret = E1CP_NBSR_AREA_OFFSET;	\
	else if (IS_MACHINE_E8C2)		\
		ret = E8C2_NBSR_AREA_OFFSET;	\
	else					\
		assert(0);				\
						\
	ret;					\
})

#define	E2K_NBSR_SIZE				\
({						\
	uint64_t ret = -1;				\
						\
	if (IS_MACHINE_E3S)			\
		ret = E3S_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_ES2)		\
		ret = ES2_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E2S)		\
		ret = E2S_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C)		\
		ret = E8C_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E1CP)		\
		ret = E1CP_NBSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C2)		\
		ret = E8C2_NBSR_AREA_SIZE;	\
	else					\
		assert(0);				\
						\
	ret;					\
})

#define	E2K_COPSR_AREA_PHYS_BASE			\
({							\
	uint64_t ret = -1;					\
							\
	if (IS_MACHINE_ES2)				\
		ret = ES2_COPSR_AREA_PHYS_BASE		\
	else if (IS_MACHINE_E2S)			\
		ret = E2S_COPSR_AREA_PHYS_BASE;		\
	else if (IS_MACHINE_E8C)			\
		ret = E8C_COPSR_AREA_PHYS_BASE;		\
	else if (IS_MACHINE_E8C2)			\
		ret = E8C2_COPSR_AREA_PHYS_BASE;	\
	else						\
		assert(0);					\
							\
	ret;						\
})

#define	E2K_COPSR_AREA_SIZE			\
({						\
	uint64_t ret = -1;				\
						\
	if (IS_MACHINE_ES2)			\
		ret = ES2_COPSR_AREA_SIZE;	\
	else if (IS_MACHINE_E2S)		\
		ret = E2S_COPSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C)		\
		ret = E8C_COPSR_AREA_SIZE;	\
	else if (IS_MACHINE_E8C2)		\
		ret = E8C2_COPSR_AREA_SIZE;	\
	else					\
		assert(0);				\
						\
	ret;					\
})

/*
 * Nodes system registers area - NSR = { NSR0 ... NSRj ... }
 * NSR is some part of common system communicator area SR
 */
#define	NODE_NSR_SIZE			E2K_NSR_AREA_SIZE
#define	THE_NODE_NSR_PHYS_BASE(node)	\
		(E2K_NSR_AREA_PHYS_BASE + (node * NODE_NSR_SIZE))
#define	NODE_NSR_PHYS_BASE()		\
		THE_NODE_NSR_PHYS_BASE(node_numa_id())

/*
 * Nodes processor system registers (north bridge)
 * NBSR = { NBSR0 ... NBSRj ... }
 * NBSR is some part of node system registers area NSR
 */
#define	NODE_NBSR_SIZE			E2K_NBSR_SIZE
#define	NODE_NBSR_OFFSET		E2K_NBSR_OFFSET
#define	THE_NODE_NBSR_PHYS_BASE(node)	\
		((unsigned char *)(THE_NODE_NSR_PHYS_BASE(node) + \
						NODE_NBSR_OFFSET))
#define	NODE_NBSR_PHYS_BASE()		\
		THE_NODE_NBSR_PHYS_BASE(node_numa_id())



#endif /* SRC_ARCH_E2K_INCLUDE_E2K_SIC_H_ */
