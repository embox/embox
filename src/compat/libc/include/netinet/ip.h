/*
 * Stub for netinet/ip.h
 *
 * @author Denis Deryugin
 * @date 3 Oct 2014
 */

#ifndef _NETINET_IP_H_
#define _NETINET_IP_H_

#define IPTOS_LOWDELAY    0x10
#define IPTOS_THROUGHPUT  0x20
#define IPTOS_RELIABILITY 0x40

#define IP_TOS            0x01
/* Socket option.
 * The obvious way to tell the IP layer not to prepend its own header
 * is by calling the setsockopt() syscall and setting the IP_HDRINCL.
 * See http://sock-raw.org/papers/sock_raw */
#define IP_HDRINCL        0x03

#endif
