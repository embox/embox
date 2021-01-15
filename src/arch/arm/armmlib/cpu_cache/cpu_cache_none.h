/**
 * @file
 * @brief
 *
 * @date    26.08.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ARMMLIB_CPU_CACHE_NONE_H_
#define ARCH_ARM_ARMMLIB_CPU_CACHE_NONE_H_

#define SRAM_NOCACHE_SECTION
#define SRAM_DEVICE_MEM_SECTION

#ifndef __ASSEMBLER__
static inline void dcache_flush_all(void) {
}
#endif

#endif /* ARCH_ARM_ARMMLIB_CPU_NOCACHE_NONE_H_ */
