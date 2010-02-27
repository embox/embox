/**
 * @file
 *
 * @date 27.05.09
 * @author Nikolay Korotky
 */
#ifndef MISC_H_
#define MISC_H_

#include <common.h>
#include <net/if_ether.h>

unsigned char *ipaddr_scan(unsigned char *addr, unsigned char *res);
unsigned char *macaddr_scan(unsigned char *addr, unsigned char *res);
void macaddr_print(unsigned char *buf, const unsigned char *addr);
int is_addr_from_net(const unsigned char *uip, const unsigned char *nip, unsigned char msk);

#endif /* MISC_H_ */
