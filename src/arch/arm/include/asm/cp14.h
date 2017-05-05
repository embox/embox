/**
 * @file
 *
 * @date 06.04.2017
 * @author Anton Bondarev
 */

#ifndef CP14_H_
#define CP14_H_

#include <hal/mem_barriers.h>

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

static inline uint32_t cp14_get_dbgdscr(void) {
	uint32_t val;
	__asm__ __volatile__ ("mrc p14,0,%0,c0,c2,2" : "=r" (val) : :"cc");
	return val;
}

static inline void cp14_set_dbgdscr(uint32_t val) {
	__asm__ __volatile__ ("mcr p14,0,%0,c0,c2,2" : :"r" (val));
}

#define CP14_GET_DBGBVR(i) \
	({ \
		uint32_t val; \
		__asm__ __volatile__ ( \
				"mrc p14, 0, %0, c0, " \
				i \
				", 4    @ get DBGBVR" : "=r" (val) : : "cc"); \
		val;\
	})

static inline uint32_t cp14_get_dbgbvr(int i) {
	switch(i) {
	case 0:
		return CP14_GET_DBGBVR("c0");
	case 1:
		return CP14_GET_DBGBVR("c1");
	case 2:
		return CP14_GET_DBGBVR("c2");
	case 3:
		return CP14_GET_DBGBVR("c3");
	default:
		return 0;
	}
}
#define CP14_SET_DBGBVR(value, i) \
	{ \
		uint32_t v = value; \
		asm volatile("mcr p14, 0, %0, c0," \
				i \
				", 4    @ set DBGBVR" \
		  : : "r" (v) : "cc"); \
		isb(); \
	}
static inline void cp14_set_dbgbvr(uint32_t val, int i) {
	switch(i) {
	case 0:
		CP14_SET_DBGBVR(val, "c0");
		break;
	case 1:
		CP14_SET_DBGBVR(val, "c1");
		break;
	case 2:
		CP14_SET_DBGBVR(val, "c2");
		break;
	case 3:
		CP14_SET_DBGBVR(val, "c3");
		break;
	default:
		return;
	}
}

#define CP14_GET_DBGBCR(i) \
	({ \
		uint32_t val; \
		__asm__ __volatile__ ( \
				"mrc p14, 0, %0, c0, " \
				i \
				", 5    @ get DBGBCR" : "=r" (val) : : "cc"); \
		val;\
	})

static inline uint32_t cp14_get_dbgbcr(int i) {
	switch(i) {
	case 0:
		return CP14_GET_DBGBCR("c0");
	case 1:
		return CP14_GET_DBGBCR("c1");
	case 2:
		return CP14_GET_DBGBCR("c2");
	case 3:
		return CP14_GET_DBGBCR("c3");
	default:
		return 0;
	}
}

#define CP14_SET_DBGBCR(value, i) \
	{ \
		uint32_t v = value; \
		asm volatile("mcr p14, 0, %0, c0," \
				i \
				", 5    @ set DBGBCR" \
		  : : "r" (v) : "cc"); \
		isb(); \
	}

static inline void cp14_set_dbgbcr(uint32_t val, int i) {
	switch(i) {
	case 0:
		CP14_SET_DBGBCR(val, "c0");
		break;
	case 1:
		CP14_SET_DBGBCR(val, "c1");
		break;
	case 2:
		CP14_SET_DBGBCR(val, "c2");
		break;
	case 3:
		CP14_SET_DBGBCR(val, "c3");
		break;
	default:
		return;
	}
}

#endif /* CP14_H_ */
