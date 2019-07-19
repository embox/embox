#ifndef HAL_ARCH_H_
# error "Do not include this file directly!"
#endif /* HAL_ARCH_H_ */

#ifndef __BYTE_ORDER__
#ifdef __ARMEB__
#define __BYTE_ORDER __BIG_ENDIAN
#define BYTE_ORDER __BYTE_ORDER
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#define BYTE_ORDER _BYTE_ORDER
#endif
#endif

#define __PLATFORM_ARCH "aarch64"
