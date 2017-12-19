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

static inline void e2k_write8(uint8_t val, void *addr) {
       asm volatile ("stb,2\t0x0, [%0] %2, %1" :
		: "r" (addr), "r" (val), "i" (6) : "memory");
}
#if 0
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
#endif
#endif /* E2K_IO_H_ */
