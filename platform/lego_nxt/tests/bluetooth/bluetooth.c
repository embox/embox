/**
 * @file
 * @brief BTinit test
 *
 * @date 26.02.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>

#include <drivers/bluetooth.h>

static bt_message_t msg;

static uint8_t tx_buff[256];
static uint8_t rx_buff[256];

EMBOX_TEST(bluetooth_test);

static int bluetooth_test(void) {
	int len;
	msg.length = 0;
	msg.type = 0x2f;
	len = bt_wrap(&msg, tx_buff);
	TRACE("bt_write start\n");
	TRACE("%x:%x",rx_buff[0], rx_buff[1]);

	while(!nxt_buttons_was_pressed()) {
		usleep(100);
	}

	nxt_bluetooth_write(tx_buff,len);
	TRACE("write done.\n");
	usleep(500);
	nxt_bluetooth_read(rx_buff, 6);
	TRACE("%x:%x",rx_buff[0], rx_buff[1]);
	TRACE("read done.\n");
	bt_unwrap(&msg, rx_buff);
	TRACE("0x%x\n", msg.type);

	while(true);

	return 0;
}
