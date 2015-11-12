/**
 * @file
 * @brief Receive commands from port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <stm32f3_discovery.h>
#include <stm32f3xx.h>

#include <agents/communication.h>
#include <kernel/time/ktime.h>

int main() {
	int res;

	HAL_Init();

	comm_init();

	while(1) {
		/* Read port */
		res = get_byte(1);

		if (res) {
			BSP_LED_Init(res);
			BSP_LED_On(res);
		} else
			continue;
		ksleep(50);
		BSP_LED_Off(res);
		ksleep(50);
	}

	return 0;
}
