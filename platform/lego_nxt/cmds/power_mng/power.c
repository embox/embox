/**
 * @file
 * @brief Power managment test of nxt
 *
 * @date 03.12.10
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <embox/cmd.h>
#include <drivers/nxt/buttons.h>
#include <drivers/nxt/avr.h>
#include <drivers/nxt/power_mng.h>

EMBOX_CMD(power_mng_cmd);

static int power_mng_cmd(int argc, char **argv) {
	printf("L:HALT R:FLASH D:SKIP\n");
	while (true) {
		int buts = nxt_buttons_pressed();
		if (buts & NXT_BUTTON_LEFT) {
			nxt_halt();
		}
		if (buts & NXT_BUTTON_RIGHT) {
			nxt_flash();
		}
		if (buts & NXT_BUTTON_DOWN) {
		    break;
		}
		usleep(200);
	}
	return 0;
}
