/**
 * @file
 *
 * @date 27.05.09
 * @author Nikolay Korotky
 */

#ifndef NET_UTIL_H_
#define NET_UTIL_H_

extern unsigned char *ipaddr_scan(const unsigned char *addr, unsigned char *res);
extern unsigned char *macaddr_scan(const unsigned char *addr, unsigned char *res);
extern void macaddr_print(unsigned char *buf, const unsigned char *addr);
extern int is_addr_from_net(const unsigned char *uip,
		    const unsigned char *nip, unsigned char msk);

#endif /* NET_UTIL_H_ */
