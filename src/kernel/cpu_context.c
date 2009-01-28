/*
 * cpu_context.c
 *
 *  Created on: Jan 26, 2009
 *      Author: anton
 */
#include "types.h"
#include "common.h"
#include "cpu_context.h"

CPU_CONTEXT cpu_context;
UINT32 register15;
const unsigned int cpu_context_addr = (unsigned int)(void *)&cpu_context;
