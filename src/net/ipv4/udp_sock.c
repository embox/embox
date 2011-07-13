/**
 * @file
 * @brief The socket for User Datagram Protocol (UDP).
 *
 * @date 06.07.11
 * @author Dmitry Zubarevich
 */

#include <net/net.h>
#include <net/in.h>
#include <embox/net/sock.h>
#include <net/udp.h>

EMBOX_NET_SOCK(SOCK_DGRAM, IPPROTO_UDP, udp_prot, inet_dgram_ops, 0, NULL);
