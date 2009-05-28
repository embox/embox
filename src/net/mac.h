/**
 * \file mac.h
 *
 * \date Mar 11, 2009
 * \author anton
 */

#ifndef MAC_H_
#define MAC_H_

#define MAC_ADDR_LEN 6

typedef struct _machdr {
	unsigned char dst_addr[MAC_ADDR_LEN];
	unsigned char src_addr[MAC_ADDR_LEN];
	unsigned short type;
}machdr;

#define MAC_HEADER_SIZE (sizeof(machdr))

#endif /* MAC_H_ */
