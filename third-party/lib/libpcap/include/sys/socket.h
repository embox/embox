#ifndef _LIBPCAP_SYS_SOCKET_H_
#define _LIBPCAP_SYS_SOCKET_H_

#include_next <sys/socket.h>

// #undef SO_ATTACH_FILTER
// #define SO_ATTACH_FILTER	26
#define SO_DETACH_FILTER	27
#define SO_TIMESTAMPNS_OLD      35
#define SO_TIMESTAMPNS		SO_TIMESTAMPNS_OLD
#define SOL_PACKET	263

#endif /* _LIBPCAP_SYS_SOCKET_H_ */
