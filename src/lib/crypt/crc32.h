/**
 * @file
 *
 * @date 01.07.09
 * @author Alexey Fomin
 */

#ifndef LIB_CRC32_H_
#define LIB_CRC32_H_

unsigned long count_crc32(unsigned char *start_addr, unsigned char *end_addr);
unsigned long crc32_accumulate(unsigned long crc32val, unsigned char *s, int len);



#endif /* LIB_CRC32_H_ */
