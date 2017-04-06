/**
 * @file	mem_barriers.h
 * @brief	Header file for memory barriers
 *
 * @author  Michele Di Giorgio
 * @date    23.11.2015
 */

#ifndef HAL_MEM_BARRIERS_H_
#define HAL_MEM_BARRIERS_H_

#include <module/embox/arch/mem_barriers.h>

#ifndef __ASSEMBLER__
extern void data_mem_barrier(void);
#endif /* __ASSEMBLER__ */

#endif /* HAL_MEM_BARRIERS_H_ */
