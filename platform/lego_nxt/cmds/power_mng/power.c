/**
 * @file
 * @brief Power managment test of nxt
 *
 * @date 03.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <unistd.h>
#include <embox/cmd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_avr.h>
#include <drivers/power_mng.h>

EMBOX_CMD(power_mng_cmd);

static int power_mng_cmd(int argc, char **argv) {
	TRACE("L:HALT R:FLASH D:SKIP\n");
	while (true) {
		int buts = nxt_buttons_was_pressed();
		if (buts & BT_LEFT) {
			nxt_halt();
		}
		if (buts & BT_RIGHT) {
			nxt_flash();
		}
		if (buts & BT_DOWN) {
		    break;
		}
		usleep(200);
	}
	return 0;
}
