/**
 * @file
 * @brief Transmit info to port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <stm32f3_discovery.h>
#include <stm32f3xx.h>

#include <agents/communication.h>
#include <kernel/time/ktime.h>

int main() {
	comm_init();

	while (1) {
		ksleep(100);
		send_byte(0x1, 1);
		ksleep(100);
		send_byte(0x2, 1);
	}

	return 0;
}
