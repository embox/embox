/**
 * @file
 * @brief
 *
 * @date 24.09.2013
 * @author Andrey Gazukin
 */

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#include <asm-generic/byteorder/little_endian.h>
#elif  __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#include <asm-generic/byteorder/big_endian.h>
#else
#  error
#endif

#endif /* BYTEORDER_H_ */
