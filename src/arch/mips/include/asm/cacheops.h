/**
 * @file
 *
 * @date May 30, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_MIPS_INCLUDE_ASM_CACHEOPS_H_
#define SRC_ARCH_MIPS_INCLUDE_ASM_CACHEOPS_H_

#ifndef __ASSEMBLER__
static inline void mips_cache(int op, const volatile void *addr) {
#ifdef __GCC_HAVE_BUILTIN_MIPS_CACHE
	__builtin_mips_cache(op, addr);
#else
	__asm__ __volatile__("cache %0, 0(%1)" : : "i"(op), "r"(addr));
#endif
}
#endif

/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define INDEX_INVALIDATE_I      0x00
#define INDEX_WRITEBACK_INV_D   0x01
#define INDEX_LOAD_TAG_I        0x04
#define INDEX_LOAD_TAG_D        0x05
#define INDEX_STORE_TAG_I       0x08
#define INDEX_STORE_TAG_D       0x09
#if defined(CONFIG_CPU_LOONGSON2)
#define HIT_INVALIDATE_I        0x00
#else
#define HIT_INVALIDATE_I        0x10
#endif
#define HIT_INVALIDATE_D        0x11
#define HIT_WRITEBACK_INV_D     0x15


/*
 * R4000SC and R4400SC-specific cacheops
 */
#define INDEX_INVALIDATE_SI     0x02
#define INDEX_WRITEBACK_INV_SD  0x03
#define INDEX_LOAD_TAG_SI       0x06
#define INDEX_LOAD_TAG_SD       0x07
#define INDEX_STORE_TAG_SI      0x0A
#define INDEX_STORE_TAG_SD      0x0B
#define CREATE_DIRTY_EXCL_SD    0x0f
#define HIT_INVALIDATE_SI       0x12
#define HIT_INVALIDATE_SD       0x13
#define HIT_WRITEBACK_INV_SD    0x17
#define HIT_WRITEBACK_SD        0x1b
#define HIT_SET_VIRTUAL_SI      0x1e
#define HIT_SET_VIRTUAL_SD      0x1f

#endif /* SRC_ARCH_MIPS_INCLUDE_ASM_CACHEOPS_H_ */
