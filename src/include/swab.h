#ifndef EMBOX_SWAB_H_
#define EMBOX_SWAB_H_

#include <compiler.h>
#include <stdint.h>

/**
 * casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to uint16_t, uint32_t, uint64_t. At least not in a portable way.
 */
#define __constant_swab16(x)                               \
	((uint16_t)((((uint16_t)(x) & (uint16_t)0x00ffU) << 8) \
	            | (((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define __constant_swab32(x)                                      \
	((uint32_t)((((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24)  \
	            | (((uint32_t)(x) & (uint32_t)0x0000ff00UL) << 8) \
	            | (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >> 8) \
	            | (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define __constant_swab64(x)                                                \
	((uint64_t)((((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56)   \
	            | (((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) \
	            | (((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) \
	            | (((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) << 8)  \
	            | (((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >> 8)  \
	            | (((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) \
	            | (((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) \
	            | (((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56)))

#define __constant_swahw32(x)                                    \
	((uint32_t)((((uint32_t)(x) & (uint32_t)0x0000ffffUL) << 16) \
	            | (((uint32_t)(x) & (uint32_t)0xffff0000UL) >> 16)))

#define __constant_swahb32(x)                                   \
	((uint32_t)((((uint32_t)(x) & (uint32_t)0x00ff00ffUL) << 8) \
	            | (((uint32_t)(x) & (uint32_t)0xff00ff00UL) >> 8)))

/**
 * swab16 - return a byteswapped 16-bit value
 * @x: value to byteswap
 *
 * swab16(0x1234) is 0x3412
 */
#if __GNUC_PREREQ(4, 8)
#define swab16(x) __builtin_bswap16(x)
#else
#define swab16(x) __constant_swab16(x)
#endif

/**
 * swab32 - return a byteswapped 32-bit value
 * @x: value to byteswap
 *
 * swab32(0x12345678) is 0x78563412
 */
#if __GNUC_PREREQ(4, 3)
#define swab32(x) __builtin_bswap32(x)
#else
#define swab32(x) __constant_swab32(x)
#endif

/**
 * swab64 - return a byteswapped 64-bit value
 * @x: value to byteswap
 * 
 * swab64(0x1020304050607080) is 0x8070605040302010
 */
#if __GNUC_PREREQ(4, 3)
#define swab64(x) __builtin_bswap64(x)
#else
#define swab64(x) __constant_swab64(x)
#endif

/**
 * swahw32 - return a word-swapped 32-bit value
 * @x: value to wordswap
 *
 * swahw32(0x12345678) is 0x56781234
 */
#define swahw32(x) __constant_swahw32(x)

/**
 * swahb32 - return a high and low byte-swapped 32-bit value
 * @x: value to byteswap
 *
 * swahb32(0x12345678) is 0x34127856
 */
#define swahb32(x) __constant_swahb32(x)

/**
 * swab16p - return a byteswapped 16-bit value from a pointer
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline uint16_t swab16p(const uint16_t *p) {
	return swab16(*p);
}

/**
 * swab32p - return a byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline uint32_t swab32p(const uint32_t *p) {
	return swab32(*p);
}

/**
 * swab64p - return a byteswapped 64-bit value from a pointer
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline uint64_t swab64p(const uint64_t *p) {
	return swab64(*p);
}

/**
 * swahw32p - return a wordswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahw32() for details of wordswapping.
 */
static inline uint32_t swahw32p(const uint32_t *p) {
	return swahw32(*p);
}

/**
 * swahb32p - return a high and low byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahb32() for details of high/low byteswapping.
 */
static inline uint32_t swahb32p(const uint32_t *p) {
	return swahb32(*p);
}

/**
 * swab16s - byteswap a 16-bit value in-place
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline void swab16s(uint16_t *p) {
	*p = swab16p(p);
}
/**
 * swab32s - byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 */
static inline void swab32s(uint32_t *p) {
	*p = swab32p(p);
}

/**
 * swab64s - byteswap a 64-bit value in-place
 * @p: pointer to a naturally-aligned 64-bit value
 */
static inline void swab64s(uint64_t *p) {
	*p = swab64p(p);
}

/**
 * swahw32s - wordswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahw32() for details of wordswapping
 */
static inline void swahw32s(uint32_t *p) {
	*p = swahw32p(p);
}

/**
 * swahb32s - high and low byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahb32() for details of high and low byte swapping
 */
static inline void swahb32s(uint32_t *p) {
	*p = swahb32p(p);
}

#endif /* EMBOX_SWAB_H_ */
