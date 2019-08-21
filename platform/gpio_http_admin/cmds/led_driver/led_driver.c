/**
 * @file
 * @brief Based on platform/stm32f4_iocontrol/cmds/libleddrv_ll.c
 *
 * @author  Alexander Kalmuk
 * @date    20.08.2019
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#include <led_driver_lib.h>

int main(int argc, char *argv[]) {
	const char *action = argv[1];

	if (action) {
		if (0 == strcmp(action, "init")) {
			return led_driver_init();
		} else if (0 == strcmp(action, "set")) {
			return led_driver_set(atoi(argv[2]));
		} else if (0 == strcmp(action, "clr")) {
			return led_driver_clear(atoi(argv[2]));
		} else if (0 == strcmp(action, "serialize_states")) {
			return led_driver_serialize();
		}
	}

	return -EINVAL;
}
