/*
 * icmp.c
 *
 *  Created on: 14.03.2009
 *      Author: sunnya
 */

typedef int (*PACKET_HANDLER)(unsigned char code, unsigned char *raw);

PACKET_HANDLER packet_handlers[256];

//returns function-handler
PACKET_HANDLER get_packet_handler(unsigned char type){
	return packet_handlers[type];
}

//---------------implementation handlers-----------------------------
int echo_answer(unsigned char code, unsigned char *raw) { //type 0

}

int echo_request(unsigned char code, unsigned char *raw) { //type 8

}
//-------------------------------------------------------------------

//set all realized handlers
int set_packet_handlers() {
	packet_handlers[0] = echo_answer;
	packet_handlers[8] = echo_request;
}

//receive packet
int icmp_receive_packet() {
	unsigned char type;
	unsigned char code;
	unsigned short header_check_summ;
	unsigned char *raw;
	PACKET_HANDLER func;

	//eth_listen(/*void *handler, unsigned short int type, ETH_LISTEN_CALLBACK callback*/);

	//put callback??

	//check type

	func = get_packet_handler(type);
	func(code, raw);
}

//send packet
int icmp_send_packet() {

	//eth_send();
}
