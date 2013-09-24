/**
 * @file
 * @brief
 *
 * @date 18.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef COMPAT_LINUX_LINUX_SWAB_H_
#define COMPAT_LINUX_LINUX_SWAB_H_

#include <stdint.h>
#include <swab.h>

/**
 * Swap bytes in 16 bit value

static inline uint16_t swab16(uint16_t x) {
	return (x >> 8) | (x << 8);
}
*/
/**
 * Swap bytes in 32 bit value

static inline uint32_t swab32(uint32_t x) {
	return (x >> 24) | ((x & 0x00FF0000) >> 8)
			| ((x & 0x0000FF00) << 8) | (x << 24);
}
*/

#endif /* COMPAT_LINUX_LINUX_SWAB_H_ */
