/**
 * @file
 * @brief Print network statistic
 *
 * @date 30.12.11
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <stdio.h>
#include <net/port.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	for(int i = 0; i < SYSTEM_PORT_MAX_NUMBER; i++) {
		if(socket_port_is_busy(i, 1)) {
			printf("TCP %d; ", i);
		} else if (socket_port_is_busy(i, 2)) {
			printf("UDP %d; ", i);
		}
	}

	return 0;
}
