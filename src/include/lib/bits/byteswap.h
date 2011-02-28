/**
 * @file
 *
 * @date 18.11.09
 * @author Nikolay Korotky
 */

#ifndef BITS_BYTESWAP_H_
#define BITS_BYTESWAP_H_

/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
	((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))

static inline unsigned short __bswap_16(unsigned short _bsx) {
	unsigned short x = 1;
	return *((unsigned char *) &x) == 0 ? _bsx : __bswap_constant_16(_bsx);
}

/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) | \
	(((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))

static inline unsigned int __bswap_32(unsigned int _bsx) {
	unsigned short x = 1;
	return *((unsigned char *) &x) == 0 ? _bsx : __bswap_constant_32(_bsx);
}

#endif /* BITS_BYTESWAP_H_ */
