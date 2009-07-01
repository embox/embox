/**
 * \file crc32.h
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#ifndef CRC32_H_
#define CRC32_H_

int check_crc32(unsigned char *start_addr, unsigned char *end_addr, unsigned long checksum);

#endif
