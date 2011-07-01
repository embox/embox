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
#include <drivers/pins.h>

static bt_message_t msg;

static uint8_t tx_buff[256];

EMBOX_TEST(nxt_bluetooth_test);

#define OLIMEX_SAM7_LED1 (1 << 17)

static int nxt_bluetooth_test(void) {
	int len;
	int res;
	msg.length = 0;
	msg.type = 0x2f;
	len = bt_wrap(&msg, tx_buff);
	TRACE("bt_write start\n");
	pin_config_output(OLIMEX_SAM7_LED1);
	pin_clear_output(OLIMEX_SAM7_LED1);

	bluetooth_write(tx_buff,len);
	usleep(250);
	res = (bt_mod_version != 0xffff );
	if (res) {
	    pin_set_output(OLIMEX_SAM7_LED1);
	}
	return (res ? 0 : -1);
}
