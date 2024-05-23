/**
 * @file
 * @brief
 *
 * @date    26.08.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ARMMLIB_CPU_CACHE_H_
#define ARCH_ARM_ARMMLIB_CPU_CACHE_H_

#define SRAM_NOCACHE_SECTION \
	__attribute__ ((section(".bss.sram_nocache")))

#define SRAM_DEVICE_MEM_SECTION \
	__attribute__ ((section(".bss.sram_device_mem")))

#ifndef __ASSEMBLER__
extern void dcache_flush_all(void);
#endif

#endif /* ARCH_ARM_ARMMLIB_CPU_CACHE_H_ */
