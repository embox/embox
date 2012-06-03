/**
 * @file
 * @brief portmapper
 * @date 03.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_PMAP_H_
#define NET_RPC_PMAP_H_

#include <types.h>
#include <net/socket.h>

#define PMAPPORT         111
#define PMAPPROG         100000
#define PMAPVERS         2
#define PMAPPROC_GETPORT 3

struct pmap {
	__u32 prog;
	__u32 vers;
	__u32 prot;
	__u32 port;
};

extern __u16 pmap_getport(struct sockaddr_in *raddr,
		__u32 prognum, __u32 versnum, __u32 protocol);

#endif /* NET_RPC_PMAP_H_ */
