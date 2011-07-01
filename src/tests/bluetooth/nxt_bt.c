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
#include <drivers/nxt_bluetooth.h>

static bt_message_t msg;

static uint8_t tx_buff[256];

EMBOX_TEST(nxt_bluetooth_test);


static int nxt_bluetooth_test(void) {
	int len;
	msg.length = 0;
	msg.type = 0x2f;
	len = bt_wrap(&msg, tx_buff);
	TRACE("bt_write start\n");

	bluetooth_write(tx_buff,len);
	usleep(250);
	return (bt_mod_version != 0xffff ? 0 : -1);
}
