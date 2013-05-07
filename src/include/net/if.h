/**
 * @file
 * @brief Global definitions for the INET interface module.
 *
 * @date 03.12.09
 * @author Nikolay Korotky
 */

#ifndef NET_IF_H_
#define NET_IF_H_

/**
 * Maximum length of interface's name (including \0)
 */
#define IFNAMSIZ        6

/**
 * Standard interface flags
 */
#define IFF_UP          0x0001 /* interface is up */
#define IFF_BROADCAST   0x0002 /* broadcast address valid */
#define IFF_DEBUG       0x0004 /* turn on debugging */
#define IFF_LOOPBACK    0x0008 /* is a loopback net */
#define IFF_POINTOPOINT 0x0010 /* interface is has p-p link */
#define IFF_NOTRAILERS  0x0020 /* avoid use of trailers */
#define IFF_RUNNING     0x0040 /* interface RFC2863 OPER_UP */
#define IFF_NOARP       0x0080 /* no ARP protocol */
#define IFF_PROMISC     0x0100 /* receive all packets */
#define IFF_ALLMULTI    0x0200 /* receive all multicast packets */
#define IFF_MULTICAST   0x1000 /* supports multicast */

#endif /* NET_IF_H_ */
