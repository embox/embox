/*
 * cpu_context.h
 *
 *  Created on: Jan 26, 2009
 *      Author: anton
 */

#ifndef CPU_CONTEXT_H_
#define CPU_CONTEXT_H_

#ifndef __ASSEMBLER__
typedef struct _CPU_CONTEXT
{
	UINT32 wim;
	UINT32 psr;
	UINT32 y;
	UINT32 tbr; //???
	UINT32 sp;
	UINT32 fp;
	UINT32 global[8];
	UINT32 r8_31[24][8];
}CPU_CONTEXT;

extern CPU_CONTEXT cpu_context;


#endif //__ASSEMBLER__
extern const unsigned int cpu_context_addr;
#endif /* CPU_CONTEXT_H_ */
