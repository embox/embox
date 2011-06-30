/**
 * @file
 * @brief BTinit test
 *
 * @date 26.02.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <string.h>

#include <drivers/bluetooth.h>
#include <drivers/nxt_buttons.h>

static bt_message_t msg;

static uint8_t tx_buff[256];

EMBOX_TEST(bluetooth_test);

extern int bt_wrap(bt_message_t *header, uint8_t *buffer);

extern uint16_t bt_mod_version;

static int bluetooth_test(void) {
	int len;
	msg.length = 0;
	msg.type = 0x2f;
	len = bt_wrap(&msg, tx_buff);
	TRACE("bt_write start\n");

	nxt_bluetooth_write(tx_buff,len);
	sleep(250);
	return (bt_mod_version != 0xffff ? 0 : -1);
}
