/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_CRC32_H_
#define __LINUX_CRC32_H_

#include <lib/crypt/crc32.h>

#define crc32(val, s, len) crc32_accumulate(val, (unsigned char *)s, len)


#endif /* __LINUX_CRC32_H_ */
