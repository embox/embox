/*
 * cpu_context.h
 *
 *  Created on: Jan 26, 2009
 *      Author: anton
 */

#ifndef CPU_CONTEXT_H_
#define CPU_CONTEXT_H_

#define WINDOWS_QUANTITY		0x8
#define GLOBAL_REGS_QUANTITY	0x8
#define LOCAL_REGS_QUANTITY		0x10

#define CPU_CONTEXT_WIM			0x00
#define CPU_CONTEXT_PSR			0x04
#define CPU_CONTEXT_Y			0x08
#define CPU_CONTEXT_TBR			0x0C
#define CPU_CONTEXT_SP			0x10
#define CPU_CONTEXT_FP			0x14
#define CPU_CONTEXT_GLOB		0x18
#define CPU_CONTEXT_LOCAL		CPU_CONTEXT_GLOB + (8 * 4)
#define CPU_CONTEXT_PC			CPU_CONTEXT_LOCAL + (WINDOWS_QUANTITY * LOCAL_REGS_QUANTITY * 4)
#define CPU_CONTEXT_NPC			CPU_CONTEXT_PC + 4

#ifndef __ASSEMBLER__

#include "types.h"

typedef struct _CPU_CONTEXT
{
	UINT32 wim;
	UINT32 psr;
	UINT32 y;
	UINT32 tbr; //???
	UINT32 sp;
	UINT32 fp;
	UINT32 global[GLOBAL_REGS_QUANTITY];
	UINT32 r8_31 [WINDOWS_QUANTITY][LOCAL_REGS_QUANTITY];
	UINT32 pc;
	UINT32 npc;
}CPU_CONTEXT;

#endif //__ASSEMBLER__





#endif /* CPU_CONTEXT_H_ */
