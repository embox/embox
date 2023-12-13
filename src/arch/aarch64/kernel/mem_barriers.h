/**
 * @file mem_barriers.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.07.2019
 */

#ifndef __ASSEMBLER__

/** 
 * DMB/DSB options:
 * 
 ** oshld  | Load-Load, Load-Store | Outer shareable
 ** oshst  |     Store-Store       | Outer shareable
 ** osh    |       Any-Any         | Outer shareable
 ** nshld  | Load-Load, Load-Store | Non-shareable
 ** nshst  |     Store-Store       | Non-shareable
 ** nsh    |       Any-Any         | Non-shareable
 ** ishld  | Load-Load, Load-Store | Inner shareable
 ** ishst  |     Store-Store       | Inner shareable
 ** ish    |       Any-Any         | Inner shareable
 ** ld     | Load-Load, Load-Store | Full system
 ** st     |     Store-Store       | Full system
 ** sy     |       Any-Any         | Full system
 */

#define isb()    __asm__ __volatile__("isb" : : : "memory")
#define dsb(opt) __asm__ __volatile__("dsb " #opt : : : "memory")
#define dmb(opt) __asm__ __volatile__("dmb " #opt : : : "memory")

static inline void data_mem_barrier(void) {
	dmb(sy);
}

#endif /* __ASSEMBLER__ */
