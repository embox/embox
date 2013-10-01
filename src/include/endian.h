/**
 * @file
 * @brief
 *
 * @date 24.09.2013
 * @author Andrey Gazukin
 */

#ifndef ENDIAN_H_
#define ENDIAN_H_

#ifdef __BYTE_ORDER__
#define __BYTE_ORDER __BYTE_ORDER__

#define	__LITTLE_ENDIAN	__ORDER_LITTLE_ENDIAN__
#define	__BIG_ENDIAN	__ORDER_BIG_ENDIAN__
#define	__PDP_ENDIAN	__ORDER_PDP_ENDIAN__

#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENGIAN    __BIG_ENDIAN
#define PDP_ENDIAN    __PDP_ENDIAN
#define BYTE_ORDER    __BYTE_ORDER
#else
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412

#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENGIAN    __BIG_ENDIAN
#define PDP_ENDIAN    __PDP_ENDIAN
#endif

#include <hal/arch.h>

#ifndef __BYTE_ORDER
#error "Byte order not defined"
#endif

#include <swab.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define htobe16 swab16
#define htobe32 swab32
#define htobe64 swab64
#define betoh16 swab16
#define betoh32 swab32
#define betoh64 swab64

#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)
#define letoh16(x) (x)
#define letoh32(x) (x)
#define letoh64(x) (x)

#elif __BYTE_ORDER == __BIG_ENDIAN

#define htole16 swab16
#define htole32 swab32
#define htole64 swab64
#define letoh16 swab16
#define letoh32 swab32
#define letoh64 swab64

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)
#define betoh16(x) (x)
#define betoh32(x) (x)
#define betoh64(x) (x)

#endif /* BYTE_ORDER */

#endif /* ENDIAN_H_ */
