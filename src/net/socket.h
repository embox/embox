/**
 * \file socket.h
 *
 * \date Mar 19, 2009
 * \author anton, sikmir
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "sock.h"

#define MAX_SOCK_NUM 4

int socket(sk_type type, sk_proto protocol);
int bind(int s, unsigned char ipaddr[4], int port);
int send(int s, const void *buf, int len);
int recv(int s, void *buf, int len);
void close(int s);
int udpsock_push(net_packet *pack);

#endif /* SOCKET_H_ */
