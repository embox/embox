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

#endif /* ENDIAN_H_ */
