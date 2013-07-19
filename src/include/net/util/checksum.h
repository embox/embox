/**
 * @file
 * @brief Checksumming functions for IP, TCP, UDP and so on.
 *
 * @date 20.03.09
 * @author Anton Bondarev
 */

#ifndef NET_UTIL_CHECKSUM_H_
#define NET_UTIL_CHECKSUM_H_

static inline unsigned long partial_sum(const void *addr, int len) {
	unsigned long sum;
	unsigned short oddbyte, *ptr;

	sum = 0;
	ptr = (unsigned short *)addr;

	while (len > 1) {
		sum += *ptr++;
		len -= 2;
	}

	if (len == 1) {
		oddbyte = 0;
		*((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
		sum += oddbyte;
	}

	return sum;
}

static inline unsigned short fold_short(unsigned long sum) {
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short) (sum & 0xffff);
}

static inline unsigned short ptclbsum(const void *addr, int len) {
	return ~fold_short(partial_sum(addr, len));
}

#endif /* NET_UTIL_CHECKSUM_H_ */
