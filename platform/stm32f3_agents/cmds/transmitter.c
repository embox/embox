/**
 * @file
 * @brief Transmit info to port
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <agents/communication.h>
#include <agents/communication.h>
#include <drivers/diag.h>
#include <kernel/task.h>
#include <kernel/time/ktime.h>
#include <kernel/thread.h>

/* Receive char via uart and turn on LED */
void *led_handler(void *arg) {
	uint8_t ch = 0;
	while (1) {
		/* Read port */
		ch = diag_getc();
		ch %= 8;
		BSP_LED_Init(ch);
		BSP_LED_On(ch);
		ksleep(50);
		BSP_LED_Off(ch);
		ksleep(50);
	}
	return NULL;
}

int main() {
	uint8_t r = 0;
	diag_init();
	BSP_PB_Init(0, 0);
	new_task(0, led_handler, NULL);

	while (1) {
		int state = BSP_PB_GetState(0);
		if (state) {
			r++;
			r %= 8;
		}
		ksleep(100);
		diag_putc(r);
		ksleep(100);
	}

	return 0;
}
