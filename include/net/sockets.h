#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "ip_addr.h"

struct sockaddr_in {
  uint8_t sin_len;
  uint8_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  uint8_t sa_len;
  uint8_t sa_family;
  char sa_data[14];
};

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AF_INET      2  /* Internet IP Protocol */

//int socket (int domain, int type, int protocol);
int connect (int s, struct sockaddr *name, socklen_t namelen);
//int bind (int s, struct sockaddr *name, socklen_t namelen);
//int close (int s);

//int send (int s, void *data, int size, unsigned int flags);

#endif //

