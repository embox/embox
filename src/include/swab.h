#ifndef _LINUX_SWAB_H
#define _LINUX_SWAB_H

//#include <linux/types.h>
//#include <linux/compiler.h>
#include <stdint.h>

/*
 * casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to uint16_t, uint32_t, uint64_t. At least not in a portable way.
 */
#define ___constant_swab16(x) ((uint16_t)(				\
	(((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |			\
	(((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define ___constant_swab32(x) ((uint32_t)(				\
	(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |		\
	(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |		\
	(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |		\
	(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((uint64_t)(				\
	(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) |	\
	(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) |	\
	(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) |	\
	(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) |	\
	(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) |	\
	(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) |	\
	(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) |	\
	(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56)))

#define ___constant_swahw32(x) ((uint32_t)(			\
	(((uint32_t)(x) & (uint32_t)0x0000ffffUL) << 16) |		\
	(((uint32_t)(x) & (uint32_t)0xffff0000UL) >> 16)))

#define ___constant_swahb32(x) ((uint32_t)(			\
	(((uint32_t)(x) & (uint32_t)0x00ff00ffUL) << 8) |		\
	(((uint32_t)(x) & (uint32_t)0xff00ff00UL) >> 8)))

/*
 * Implement the following as inlines, but define the interface using
 * macros to allow constant folding when possible:
 * ___swab16, ___swab32, ___swab64, ___swahw32, ___swahb32
 */

static inline uint16_t __fswab16(uint16_t val) {
#ifdef __arch_swab16
	return __arch_swab16(val);
#else
	return ___constant_swab16(val);
#endif
}

static inline uint32_t __fswab32(uint32_t val) {
#ifdef __arch_swab32
	return __arch_swab32(val);
#else
	return ___constant_swab32(val);
#endif
}

static inline  uint64_t __fswab64(uint64_t val) {
#ifdef __arch_swab64
	return __arch_swab64(val);
#elif defined(__SWAB_64_THRU_32__)
	uint32_t h = val >> 32;
	uint32_t l = val & ((1ULL << 32) - 1);
	return (((uint64_t)__fswab32(l)) << 32) | ((uint64_t)(__fswab32(h)));
#else
	return ___constant_swab64(val);
#endif
}

static inline  uint32_t __fswahw32(uint32_t val) {
#ifdef __arch_swahw32
	return __arch_swahw32(val);
#else
	return ___constant_swahw32(val);
#endif
}

static inline  uint32_t __fswahb32(uint32_t val) {
#ifdef __arch_swahb32
	return __arch_swahb32(val);
#else
	return ___constant_swahb32(val);
#endif
}

/**
 * __swab16 - return a byteswapped 16-bit value
 * @x: value to byteswap
 */
#define __swab16(x)				\
	(__builtin_constant_p((uint16_t)(x)) ?	\
	___constant_swab16(x) :			\
	__fswab16(x))

/**
 * __swab32 - return a byteswapped 32-bit value
 * @x: value to byteswap
 */
#define __swab32(x)				\
	(__builtin_constant_p((uint32_t)(x)) ?	\
	___constant_swab32(x) :			\
	__fswab32(x))

/**
 * __swab64 - return a byteswapped 64-bit value
 * @x: value to byteswap
 */
#define __swab64(x)				\
	(__builtin_constant_p((uint64_t)(x)) ?	\
	___constant_swab64(x) :			\
	__fswab64(x))

/**
 * __swahw32 - return a word-swapped 32-bit value
 * @x: value to wordswap
 *
 * __swahw32(0x12340000) is 0x00001234
 */
#define __swahw32(x)				\
	(__builtin_constant_p((uint32_t)(x)) ?	\
	___constant_swahw32(x) :		\
	__fswahw32(x))

/**
 * __swahb32 - return a high and low byte-swapped 32-bit value
 * @x: value to byteswap
 *
 * __swahb32(0x12345678) is 0x34127856
 */
#define __swahb32(x)				\
	(__builtin_constant_p((uint32_t)(x)) ?	\
	___constant_swahb32(x) :		\
	__fswahb32(x))

/**
 * __swab16p - return a byteswapped 16-bit value from a pointer
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline uint16_t __swab16p(const uint16_t *p) {
#ifdef __arch_swab16p
	return __arch_swab16p(p);
#else
	return __swab16(*p);
#endif
}

/**
 * __swab32p - return a byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline uint32_t __swab32p(const uint32_t *p) {
#ifdef __arch_swab32p
	return __arch_swab32p(p);
#else
	return __swab32(*p);
#endif
}

/**
 * __swab64p - return a byteswapped 64-bit value from a pointer
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline uint64_t __swab64p(const uint64_t *p) {
#ifdef __arch_swab64p
	return __arch_swab64p(p);
#else
	return __swab64(*p);
#endif
}

/**
 * __swahw32p - return a wordswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahw32() for details of wordswapping.
 */
static inline uint32_t __swahw32p(const uint32_t *p) {
#ifdef __arch_swahw32p
	return __arch_swahw32p(p);
#else
	return __swahw32(*p);
#endif
}

/**
 * __swahb32p - return a high and low byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahb32() for details of high/low byteswapping.
 */
static inline uint32_t __swahb32p(const uint32_t *p) {
#ifdef __arch_swahb32p
	return __arch_swahb32p(p);
#else
	return __swahb32(*p);
#endif
}

/**
 * __swab16s - byteswap a 16-bit value in-place
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline void __swab16s(uint16_t *p) {
#ifdef __arch_swab16s
	__arch_swab16s(p);
#else
	*p = __swab16p(p);
#endif
}
/**
 * __swab32s - byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline void __swab32s(uint32_t *p) {
#ifdef __arch_swab32s
	__arch_swab32s(p);
#else
	*p = __swab32p(p);
#endif
}

/**
 * __swab64s - byteswap a 64-bit value in-place
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline void __swab64s(uint64_t *p) {
#ifdef __arch_swab64s
	__arch_swab64s(p);
#else
	*p = __swab64p(p);
#endif
}

/**
 * __swahw32s - wordswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahw32() for details of wordswapping
 */
static inline void __swahw32s(uint32_t *p) {
#ifdef __arch_swahw32s
	__arch_swahw32s(p);
#else
	*p = __swahw32p(p);
#endif
}

/**
 * __swahb32s - high and low byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahb32() for details of high and low byte swapping
 */
static inline void __swahb32s(uint32_t *p) {
#ifdef __arch_swahb32s
	__arch_swahb32s(p);
#else
	*p = __swahb32p(p);
#endif
}

# define swab16 __swab16
# define swab32 __swab32
# define swab64 __swab64
# define swahw32 __swahw32
# define swahb32 __swahb32
# define swab16p __swab16p
# define swab32p __swab32p
# define swab64p __swab64p
# define swahw32p __swahw32p
# define swahb32p __swahb32p
# define swab16s __swab16s
# define swab32s __swab32s
# define swab64s __swab64s
# define swahw32s __swahw32s
# define swahb32s __swahb32s

#endif /* _LINUX_SWAB_H */
