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

#include "common.h"
#include "tftp.h"
#include "net.h"
#include "eth.h"
#include "inet_sock.h"
#include "net_pack_manager.h"
#include "ip_v4.h"

#define TFTP_ADDRESS_TO_SAVE	0x80000000

#define TFTP_FULL_PACKET_LENGTH	512

int offset = 0;


int tftp_send_ack (void *ifdev, unsigned char dstaddr[4], int block_number) {
	net_packet *pack = net_packet_alloc();
	if (pack == 0)
		return -1;
	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);

	TFTP_ACK_PACKET ack_packet;
	ack_packet.op  = TFTP_ACK;
	ack_packet.block = block_number;

	int i;
	char *data = (char *) &ack_packet;
	for (i = 0; i < strlen(data); i++) {
		pack->data[i] = data[i];
	}
	pack->len = strlen(data);
	pack->protocol = IP_PROTOCOL_TYPE;

	//inet_sock??

	//udp_trans??

	return eth_send(pack) /*right?*/;
}

int tftp_send_rrq_request(void *ifdev, unsigned char dstaddr[4], char *filename, char *mode) {
	net_packet *pack = net_packet_alloc();
	if (pack == 0)
		return -1;
	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);

	TFTP_RQ_PACKET rrq_packet;
	rrq_packet.op = TFTP_RRQ;
	rrq_packet.filename = filename;
	rrq_packet.mode = mode;

	int i;
	char *data = (char *) &rrq_packet;
	for (i = 0; i < strlen(data); i++) {
		pack->data[i] = data[i];
	}
	pack->len = strlen(data);
	pack->protocol = IP_PROTOCOL_TYPE;

	// too

	return eth_send(pack);
}

int tftp_received_type_error() {
	DEBUG("TFTP session error. Impossible message type\n");
	DEBUG("Session closed\n");
}

int tftp_received_error_handler(net_packet *pack){
	TFTP_ERR_PACKET *data = (TFTP_ERR_PACKET *) (pack->data);
	DEBUG("TFTP session error. Error by server\n");
	DEBUG("Error code %d, message %s\n", data->errcode, data->msg);
	DEBUG("Session closed\n");
}

int tftp_received_data_handler(net_packet *pack) {
	TFTP_DATA_PACKET *data_packet = (TFTP_DATA_PACKET *) (pack->data);
	char *current_address  = (char *)(TFTP_ADDRESS_TO_SAVE + offset);
	//save data
	int i;
	for (i = 0; i < strlen(data_packet->data); i++) {
		current_address[i] = ((char *)(data_packet->data))[i];
	}
	offset +=TFTP_FULL_PACKET_LENGTH;

	//tftp_send_ack(data->block);

	if ((pack->h.uh->len) < (sizeof(udphdr) + TFTP_FULL_PACKET_LENGTH )) {
		//last packet
		DEBUG("File transfer finished. Session closed\n");
		offset = 0;
	}
}
int tftp_received_packet(net_packet *pack) {
	//watch first 2 bytes to find message type
	HWRD type;
	type = *(HWRD *)(pack->data);
	if 		( 0x5 == type ) 	//error-message
		tftp_received_error_handler(pack);
	else if ( 0x3 == type )		//data-message
		tftp_received_data_handler(pack);
	else						//session error
		tftp_received_type_error();
}
