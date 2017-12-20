/**
 * @file io.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2017
 */

#ifndef E2K_IO_H_
#define E2K_IO_H_

#include <stdint.h>

#define WMB_AFTER_ATOMIC	".word 0x00008001\n" \
				".word 0x30000084\n"

#define __e2k_atomic32_add(__val, __addr) \
({ \
	int __rval; \
	asm volatile ("\n1:" \
			  "\n\tldw,0 %[addr], %[rval], mas=0x7" \
			  "\n\tadds %[rval], %[val], %[rval]" \
			  "\n\t{"\
			  "\n\tstw,2 %[addr], %[rval], mas=0x2" \
			  "\n\tibranch 1b ? %%MLOCK" \
			  "\n\t}" \
			  WMB_AFTER_ATOMIC \
			  : [rval] "=&r" (__rval), [addr] "+m" (*(__addr)) \
			  : [val] "ir" (__val) \
			  : "memory"); \
	__rval; \
})

static void e2k_wait_all(void) {
	_Pragma ("no_asm_inline")
	asm volatile ("wait \ttrap = %0, ma_c = %1, fl_c = %2, ld_c = %3, "
			"st_c = %4, all_e = %5, all_c = %6"
			: : "i" (0), "i" (1), "i" (1), "i" (0), "i" (0), "i" (1), "i" (1) : "memory");
}

static void wmb() {
	asm volatile (	".word 0x00008001\n"
			".word 0x30000084\n"
			::: "memory");
}

static inline void e2k_write8(uint8_t val, void *addr) {
       asm volatile ("stb,2\t0x0, [%0] %2, %1" :
		: "r" (addr), "r" (val), "i" (6) : "memory");
}

static inline void e2k_write16(uint16_t val, void *addr) {
       asm volatile ("sth,2\t0x0, [%0] %2, %1" :
		: "r" (addr), "r" (val), "i" (6) : "memory");
}

static inline void e2k_write32(uint32_t val, void *addr) {
       asm volatile ("stw,2\t0x0, [%0] %2, %1" :
		: "r" (addr), "r" (val), "i" (6) : "memory");
}

static inline void e2k_write64(uint64_t val, void *addr) {
       asm volatile ("std,2\t0x0, [%0] %2, %1" :
		: "r" (addr), "r" (val), "i" (6) : "memory");
}

static inline uint8_t e2k_read8(void *addr) {
	register uint8_t res;
	asm volatile ("ldb,2 \t0x0, [%1] %2, %0" :
		"=r" (res) : "r" (addr), "i" (6));
	return res;
}

static inline uint16_t e2k_read16(void *addr) {
	register uint16_t res;
	asm volatile ("ldh,2 \t0x0, [%1] %2, %0" :
		"=r" (res) : "r" (addr), "i" (6));
	return res;
}

static inline uint32_t e2k_read32(void *addr) {
	register uint32_t res;
	asm volatile ("ldw,2 \t0x0, [%1] %2, %0" :
		"=r" (res) : "r" (addr), "i" (6));
	return res;
}

static inline uint64_t e2k_read64(void *addr) {
	register uint64_t res;
	asm volatile ("ldd,2 \t0x0, [%1] %2, %0" :
		"=r" (res) : "r" (addr), "i" (6));
	return res;
}

#endif /* E2K_IO_H_ */
