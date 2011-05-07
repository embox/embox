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
// XXX defined but not used
#if 0
static uint8_t rx_buff[256];
#endif
static char hello[] = "Hello world\n\r";
static int hello_len = 12;

EMBOX_TEST(bluetooth_test);

extern int bt_wrap(bt_message_t *header, uint8_t *buffer);

static int bluetooth_test(void) {
	int len;
	msg.length = 0;
	msg.type = 0x2f;
	len = bt_wrap(&msg, tx_buff);
	TRACE("bt_write start\n");

	while (1) {
		int buttons = 0;
		while (!(buttons = nxt_buttons_was_pressed())) {
			usleep(250);
		}
		if (buttons & BT_ENTER) {
			nxt_bluetooth_write(tx_buff,len);
		}
		if (buttons & BT_DOWN) {
			nxt_bluetooth_write((uint8_t *) hello, hello_len);
		}
	}

	return 0;
}
