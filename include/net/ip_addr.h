/**
 * @file ip_addr.h
 *
 * @date 01.10.09
 * @author Andrey Baboshin
 */
#ifndef __IP_ADDR_H__
#define __IP_ADDR_H__

/* for LWIP */
#if 0
struct ip_addr {
	unsigned int addr __attribute__((packed));
} __attribute__((packed));

extern const struct ip_addr ip_addr_any;
extern const struct ip_addr ip_addr_broadcast;
#endif

#define IP_ADDR_ANY ((struct ip_addr *)&ip_addr_any)
#define IP_ADDR_BROADCAST ((struct ip_addr *)&ip_addr_broadcast)

//#define INADDR_NONE    ((uint32_t) 0xffffffff)  /* 255.255.255.255 */
//#define INADDR_LOOPBACK    ((uint32_t) 0x7f000001)  /* 127.0.0.1 */

#endif /* __IP_ADDR_H__ */
