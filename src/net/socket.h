/*
 * socket.h
 *
 *  Created on: Mar 19, 2009
 *      Author: anton
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "sock.h"

#define MAX_SOCK_NUM 0x10

int socket(sk_type type, sk_proto protocol);
int bind(int s, int port);
int send(int s, const void *buf, int len);
int recv(int s, void *buf, int len);
void close(int s);
void udpsock_push(net_packet *pack);

#endif /* SOCKET_H_ */
