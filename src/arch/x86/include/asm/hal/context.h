#ifndef HAL_CONTEXT_H_
# error "Do not include this file directly!"
#endif /* HAL_CONTEXT_H_ */

#include <types.h>

struct context {
	/* 0x00 */uint32_t eip; /**< TODO */
	/* 0x04 */uint32_t ebx; /**< TODO */
	/* 0x08 */uint32_t edi; /**< TODO */
	/* 0x0c */uint32_t esi; /**< TODO */
	/* 0x10 */uint32_t ebp; /**< TODO */
	/* 0x14 */uint32_t esp; /**< TODO */
};
