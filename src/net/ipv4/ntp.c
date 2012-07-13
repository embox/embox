/*
 * @brief NTP implementation over IPv4
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <stdlib.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/ntp.h>

#define NEWPS 0
#define DSCRD 1
#define FXMIT 2
#define MANY  3
#define NEWBC 4
#define ERR   5
#define PROC  6

static int ntp_table[7][5] = {
		   /* nopeer  */   { NEWPS, DSCRD, FXMIT, MANY, NEWBC  },
		   /* active  */   { PROC,  PROC,  DSCRD, DSCRD, DSCRD },
		   /* passv   */   { PROC,  ERR,   DSCRD, DSCRD, DSCRD },
		   /* client  */   { DSCRD, DSCRD, DSCRD, PROC,  DSCRD },
		   /* server  */   { DSCRD, DSCRD, DSCRD, DSCRD, DSCRD },
		   /* bcast   */   { DSCRD, DSCRD, DSCRD, DSCRD, DSCRD },
		   /* bclient */   { DSCRD, DSCRD, DSCRD, DSCRD, PROC  }
		   };

int ntp_proc(struct ntphdr *pack) {
	return 0;
}

int ntp_receive(struct ntphdr *client_pack, struct ntphdr *server_pack) {
	__u8 client_mode = get_mode(client_pack);
	__u8 server_mode = get_mode(server_pack);

	switch (ntp_table[client_mode][server_mode]) {
	case PROC:
		ntp_proc(server_pack);
		break;
	default:
		break;
	}

	return 0;
}
