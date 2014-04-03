/**
 * @file
 * @brief
 *
 * @date 25.09.2013
 * @author Andrey Gazukin
 */

#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <endian.h>

#ifdef __BYTE_ORDER__
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#include <linux/byteorder/little_endian.h>
# elif  __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#include <linux/byteorder/big_endian.h>
# else
#  error
# endif
#endif

#endif /* BYTEORDER_H_ */
