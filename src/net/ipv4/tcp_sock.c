/**
 * @file
 * @brief Implementation of the socket for Transmission Control Protocol(TCP).
 *
 * @date 06.07.11
 * @author Dmitry Zubarevich
 */

#include <net/net.h>
#include <net/ip.h>
#include <embox/net/sock.h>

EMBOX_NET_SOCK(SOCK_STREAM, IPPROTO_TCP, tcp_prot, inet_stream_ops, 0, &inet_family_ops);
