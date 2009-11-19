#ifndef __IP_ADDR_H__
#define __IP_ADDR_H__

struct ip_addr {
	unsigned int addr __attribute__((packed));
} __attribute__((packed));

//struct in_addr {
//  u32_t s_addr;
//};

extern const struct ip_addr ip_addr_any;
extern const struct ip_addr ip_addr_broadcast;

#define IP_ADDR_ANY ((struct ip_addr *)&ip_addr_any)
#define IP_ADDR_BROADCAST ((struct ip_addr *)&ip_addr_broadcast)

#define INADDR_NONE    ((u32_t) 0xffffffff)  /* 255.255.255.255 */
//#define INADDR_LOOPBACK    ((u32_t) 0x7f000001)  /* 127.0.0.1 */


#endif //

