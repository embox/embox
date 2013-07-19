/**
 * @file
 * @brief portmapper
 * @date 03.06.12
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_RPC_PMAP_H_
#define NET_LIB_RPC_PMAP_H_

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Prototypes */
struct sockaddr_in;
struct xdr;

#define PMAPPORT         111
#define PMAPPROG         100000
#define PMAPVERS         2
#define PMAPPROC_GETPORT 3

struct pmap {
	uint32_t prog;
	uint32_t vers;
	uint32_t prot;
	uint32_t port;
};

extern uint16_t pmap_getport(struct sockaddr_in *raddr,
		uint32_t prognum, uint32_t versnum, uint32_t protocol);

extern int xdr_pmap(struct xdr *xs, struct pmap *pmp);

#endif /* NET_LIB_RPC_PMAP_H_ */
