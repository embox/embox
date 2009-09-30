/**
 * \file tftp.c
 *
 * \date Sep 29, 2009
 * \author ababoshin
 */

#include "string.h"
#include "conio/conio.h"
#include "common.h"
#include "net/tftp.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "net/inet_sock.h"
#include "net/net_pack_manager.h"
#include "net/in.h"
#include "net/socket.h"

//#define TFTP_ADDRESS_TO_SAVE  0x80000000
	int
tftp_client_get (const char *const filename,
		 const ip_addr_t server,
		 const int port, char *buf, int len, const int mode, int *const err) {
	TRACE("tftp client started\n");
	int result = 0;
	int s = -1;
	int actual_len, data_len;
	socklen_t from_len, recv_len;
	static int get_port = 7700;
	int error;

	struct sockaddr local_addr, from_addr;
	char data[SEGSIZE + sizeof (struct tftphdr)];
	struct tftphdr *hdr = (struct tftphdr *) data;
	const char *fp;
	char *cp, *bp;
	unsigned short last_good_block = 0;
	int total_timeouts = 0;

	struct sockaddr_in saddr;
	struct sockaddr_in laddr;

	*err = 0;		// Just in case

	// Create initial request
	hdr->th_opcode = htons (RRQ);	// Read file
	cp = (char *) &hdr->th_stuff;
	fp = filename;
	while (*fp) {
		*cp++ = *fp++;
	}
	*cp++ = '\0';
	if (mode == TFTP_NETASCII) {
		fp = "NETASCII";
	}
	else if (mode == TFTP_OCTET) {
		fp = "OCTET";
	}
	else {
		*err = TFTP_INVALID;
		return -1;
	}
	while (*fp)
		*cp++ = *fp++;
	*cp++ = '\0';

	TRACE("call socket\n");
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		LOG_ERROR ("tftp socket failed");
		return -1;
	}

	memset (&laddr, 0, sizeof (struct sockaddr_in));
	laddr.sin_port = htons (get_port++);
	laddr.sin_addr.s_addr = INADDR_ANY;

	memset (&saddr, 0, sizeof (struct sockaddr_in));
	saddr.sin_port = htons (port);
	memcpy (&saddr.sin_addr.s_addr, server, sizeof (ip_addr_t));

	TRACE("call bind\n");
	if (bind (s, (struct sockaddr *) &saddr, sizeof (saddr)) < 0) {
		LOG_ERROR ("tftp bind failed");
		return -1;
	}

	TRACE("call send\n");
	// Send request
	if (send (s, data, (int) (cp - data), 0) < 0) {
		LOG_ERROR ("tftp send failed");
		return -1;
	}

	TRACE("try to read tftp data\n");

	// wait a bit
//	sleep (100);

	// Read data
	bp = buf;

	while (!empty_socket (s)) {
		recv_len = sizeof (data);
		from_len = sizeof (from_addr);
		if ((data_len = recv (s, data, recv_len, 0)) < 0) {
			LOG_ERROR ("tftp recv failed");
			return -1;
		}
		if (ntohs (hdr->th_opcode) == DATA) {
			actual_len = 0;
			if (ntohs (hdr->th_block) == (last_good_block + 1)) {
				cp = hdr->th_data;
				data_len -= 4;	/* Sizeof TFTP header */
				actual_len = data_len;
				result += actual_len;
				while (data_len-- > 0) {
					if (len-- > 0) {
						*bp++ = *cp++;
					} else {
						LOG_ERROR ("tftp buffer overflow");
						close (s);
						return -1;
					}
				}
				last_good_block++;
			} else {
				// To prevent an out-of-sequence packet from
				// terminating transmission prematurely, set
				// actual_len to a full size packet.
				actual_len = SEGSIZE;
			}
			// Send out the ACK
			hdr->th_opcode = htons (ACK);
			hdr->th_block = htons (last_good_block);
			if (send (s, data, 4, 0) < 0) {
				LOG_ERROR ("tftp send ack error");
				close (s);
				return -1;
			}
			// A short packet marks the end of the file.
			if ((actual_len >= 0) && (actual_len < SEGSIZE)) {
				// End of data
				close (s);
				return result;
			}
		} else if (ntohs (hdr->th_opcode) == ERROR) {
			LOG_ERROR ("tftp got an error no = %d\n", ntohs (hdr->th_code));
			close (s);
			return -1;
		} else {
			LOG_ERROR ("tftp received strange packet\n");
			close (s);
			return -1;
		}
	}
}


