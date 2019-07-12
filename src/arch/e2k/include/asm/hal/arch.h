#ifndef HAL_ARCH_H_
# error "Do not include this file directly!"
#endif /* HAL_ARCH_H_ */

#ifndef __BYTE_ORDER__

#ifndef __ORDER_LITTLE_ENDIAN__
#define __ORDER_LITTLE_ENDIAN__ _LITTLE_ENDIAN
#endif

#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __FLOAT_WORD_ORDER__ __ORDER_LITTLE_ENDIAN__

#define __ORDER_BIG_ENDIAN__ 0
#endif

#define __BYTE_ORDER __ORDER_LITTLE_ENDIAN__
#define __PLATFORM_ARCH "e2k"
