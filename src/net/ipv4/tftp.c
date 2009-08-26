/**
 * \file tftp.c
 *
 * \date May 16, 2009
 * \author sunnya
 */
/////////////////////////////////////Comment!!////////////////////////////////////
//!! send functions contain errors (for the present i don't know
//	 how they are linked with udp)
//	 +(error?): DEBUGs

#include "string.h"
#include "conio/conio.h"
#include "common.h"
#include "net/tftp.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "net/inet_sock.h"
#include "net/net_pack_manager.h"
#include "net/ip.h"

#define TFTP_ADDRESS_TO_SAVE	0x80000000

#define TFTP_FULL_PACKET_LENGTH	512

int offset = 0;

////////primitives realization///////////////////
int tftp_send_ack (void *ifdev, unsigned char srcaddr[4], unsigned char dstaddr[4], int block_number) {
	net_packet *pack = net_packet_alloc();
	if (pack == 0)
		return -1;
	pack->ifdev = ifdev;
	pack->netdev = (struct _net_device *)ifdev_get_netdevice(ifdev);

	tftphdr header;
	header.th_opcode  = TFTP_ACK;
	header.th_u.tu_block = block_number;

	int i;
	char *data = (char *) &header;
	for (i = 0; i < strlen(data); i++) {
		pack->data[i] = data[i];
	}
	pack->len += strlen(data);
	pack->protocol = ETH_P_IP;

	memcpy(pack->nh.iph->daddr, dstaddr, sizeof(pack->nh.iph->daddr));
	memcpy(pack->nh.iph->saddr, srcaddr, sizeof(pack->nh.iph->saddr));

	return udpsock_push(pack);
	//inet_sock??
	//udp_trans??   <<-- NO, tftp is application layer, not transport.

	/*TODO: it's something wrong.
	 inside tftp_send(args) and tftp_receive(args):
	         create udp socket
	         ..todo..
	         close socket
	 and use tftphdr insist of TFTP_RQ_PACKET, etc.
	*/

}

int tftp_send_rrq_request(void *ifdev, unsigned char srcaddr[4], unsigned char dstaddr[4], char *filename, char *mode) {
	net_packet *pack = net_packet_alloc();
	if (pack == NULL) {
		return -1;
	}
	pack->ifdev = ifdev;
	pack->netdev = ifdev_get_netdevice(ifdev);

	tftphdr header;
	header.th_opcode = TFTP_RRQ;

	char *stuff = (char *) &(header.th_u.tu_stuff);
	while(*stuff++ = *filename++);
	*stuff++ = '\0';
	while (*stuff++ = *mode++);
	*stuff++ = '\0';

    int i;

	char *data = (char *) &header;
	for (i = 0; i < strlen(data); i++) {
		pack->data[i] = data[i];
	}
	pack->len = strlen(data);
	pack->protocol = ETH_P_IP;

	memcpy(pack->nh.iph->daddr, dstaddr, sizeof(pack->nh.iph->daddr));
	memcpy(pack->nh.iph->saddr, srcaddr, sizeof(pack->nh.iph->saddr));

	return udpsock_push(pack);
}

int tftp_received_type_error() {
	LOG_DEBUG("TFTP session error. Impossible message type\n");
	LOG_DEBUG("Session closed\n");
}

int tftp_received_error_handler(net_packet *pack){
	tftphdr *data = (tftphdr *) (&pack->data);
	LOG_DEBUG("TFTP session error. Error by server\n");
	LOG_DEBUG("Error code %d, message %s\n", data->th_u.tu_code, data->th_data);
	LOG_DEBUG("Session closed\n");
}

int tftp_received_data_handler(net_packet *pack) {
	tftphdr *data_packet = (tftphdr *) (pack->data);
	char *current_address  = (char *)(TFTP_ADDRESS_TO_SAVE + offset);
	//save data
	int i;
	for (i = 0; i < strlen(data_packet->th_data); i++) {
		current_address[i] = ((char *)(data_packet->th_data))[i];
	}
	offset +=TFTP_FULL_PACKET_LENGTH;

	//tftp_send_ack(data->block);

//	if ((pack->h.uh->len) < (sizeof(udphdr) + TFTP_FULL_PACKET_LENGTH )) /*size?*/{
//		//last packet
//		LOG_DEBUG("File transfer finished. Session closed\n");
//		offset = 0;
//	}
}

int tftp_received_packet(net_packet *pack) {
	//watch first 2 bytes to find message type
	HWRD type;
	type = *(HWRD *)(pack->data);
	if ( 0x5 == type ) {	//error-message
		tftp_received_error_handler(pack);
	} else if ( 0x3 == type ) {	//data-message
		tftp_received_data_handler(pack);
	} else {						//session error
		tftp_received_type_error();
	}
}

///////////////////////////////////////////////////////////
//int load_file() { //  <-- i think so
//    //..todo..
//    int i, sk;
//    sk = socket(SOCK_DGRAM, UDP);
//    if (sk < 0) {
//        LOG_ERROR("opening socket\n");
//        return -1;
//    }
//    if (bind(sk, addr, port)) {
//        LOG_ERROR("binding socket\n");
//        return -1;
//    }
//    //       while (1) {...          //  using primitives
//    close(sk);
//    return 0;
//}
