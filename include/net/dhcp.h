/**
 * \file dhcp.h
 *
 * \date 27.03.2009
 * \author fomka
 */

#ifndef DHCP_H_
#define DHCP_H_

#define BOOTREQUEST 0x1 // not sure
#define BOOTREPLY 0x2   // not sure

#include "types.h"

int dhcpDiscover();

typedef struct {
	/** Message type
	 * 2 possible values:
	 * BOOTREQUEST (from client to server)
	 * BOOTREPLY (from server to client)
	 */
	BYTE op;

	BYTE htype;
	BYTE hlen;

	BYTE hops;
	BYTE xid;


	HWRD secs;
	HWRD flags;

	WORD ciaddr;
	WORD yiaddr;
	WORD siaddr;
	WORD giaddr;

	/** hardware address
	 * Regularly MAC-address
	 */
	WORD chaddr[4];
	/** Server name. (optional)
	 * Zero-ending string
	 */
	char sname[64];

	/** File name (for remote access)
	 * Zero-ending string.
	 */
	char file[128];

	/** Other options (optional)
	 * we don't use
	 */
	// _type options;
} DHCP_PACKET;

#endif /* DHCP_H_ */
