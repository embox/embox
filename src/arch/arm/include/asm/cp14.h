/**
 * @file
 *
 * @date 06.04.2017
 * @author Anton Bondarev
 */

#ifndef CP14_H_
#define CP14_H_

static inline uint32_t cp14_get_dbgdidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p14, 0, %0, c0, c0, 0    @ get DBGDIDR" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t cp14_get_dbgdrar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p14, 0, %0, c1, c0, 0    @ get DBGDRAR" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t cp14_get_dbgdsar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p14, 0, %0, c2, c0, 0    @ get DBGDSAR" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t cp14_get_dbgbvr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p14, 0, %0, c0, c0, 5    @ get DBGBVR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp14_set_dbgbvr(uint32_t val) {
	asm volatile("mcr p14, 0, %0, c0, c0, 5    @ set DBGBVR"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp14_get_dbgbcr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p14, 0, %0, c2, c0, 4    @ get DBGBCR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp14_set_dbgbcr(uint32_t val) {
	asm volatile("mcr p14, 0, %0, c0, c0, 4    @ set DBGBCR"
	  : : "r" (val) : "cc");
	isb();
}

#endif /* CP14_H_ */
