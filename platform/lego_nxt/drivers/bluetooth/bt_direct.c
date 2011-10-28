/**
 * @file
 *
 * @brief
 *
 * @date 28.10.2011
 * @author Anton Bondarev
 */


//static enum {
//	COMM_HEADER,
//	COMM_BODY,
//} reader_state;


#define DC_BUFF_SIZE 0x20 /* lego_nxt direct command maximum length */

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

static void send_to_net(uint8_t *data, int len) {
	struct sk_buff *skb = alloc_skb(len, 0);
	memcpy(skb->data, data, len);
	netif_rx(skb);
	return;
}

static int direct_wait_body(int msg, uint8_t *buff);
static int direct_wait_length(int msg, uint8_t *buff) {
	size = handle_size(buff);

	if (size > DC_BUFF_SIZE - MSG_SIZE_BYTE_CNT) {
		//TODO error length
	}
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_wait_body);
	bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
	return 0;
}

static int direct_wait_body(int msg, uint8_t *buff) {
	send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_wait_length);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
}

//static int direct_comm_handle(int msg, uint8_t *buff) {
////TODO NOT HERE
////	if (msg == BT_DRV_MSG_CONNECTED) {
////	    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
////	    return;
////	}
//	if (msg == BT_DRV_MSG_READ) {
//	    switch (reader_state) {
//	    case COMM_HEADER:
//		    reader_state = COMM_BODY;
//		    size = handle_size(buff);
//
//		    if(size > DC_BUFF_SIZE - MSG_SIZE_BYTE_CNT) {
//		    	//TODO error length
//		    }
//		    bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
//
//		    break;
//	    case COMM_BODY:
//		    reader_state = COMM_HEADER;
//
//		    send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
//
//		    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
//
//		    break;
//	    default:
//		    break;
//	    }
//	}
//	return 0;
//}

static int nxt_direct_comm_init(void) {
	//reader_state = COMM_HEADER;// NOT CONNECT
	//CALLBACK_REG(bluetooth_uart, (callback_t) direct_comm_handle);
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_wait_length);
	return 0;
}


