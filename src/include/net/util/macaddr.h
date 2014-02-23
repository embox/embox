/**
 * @file
 *
 * @date 27.05.09
 * @author Nikolay Korotky
 */

#ifndef NET_UTIL_MACADDR_H_
#define NET_UTIL_MACADDR_H_

extern unsigned char *macaddr_scan(const unsigned char *addr, unsigned char *res);
extern void macaddr_print(unsigned char *buf, const unsigned char *addr);

#define MACADDR_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACADDR_FMT_ARG(x) (x)[0],(x)[1],(x)[2],(x)[3],(x)[4],(x)[5]

#endif /* NET_UTIL_MACADDR_H_ */
