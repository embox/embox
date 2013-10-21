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
#define be16toh swab16
#define be32toh swab32
#define be64toh swab64

#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)
#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)

#elif __BYTE_ORDER == __BIG_ENDIAN

#define htole16 swab16
#define htole32 swab32
#define htole64 swab64
#define le16toh swab16
#define le32toh swab32
#define le64toh swab64

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)
#define be16toh(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)

#endif /* BYTE_ORDER */

#endif /* ENDIAN_H_ */
