
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "stm32746g_discovery.h"

static void stm32f7_delay(int delay) {
	while (delay--)
		;
}

static void init_leds() {
	BSP_LED_Init(LED1);
}

int main(int argc, char *argv[]) {
	printf("STM32F7 test start\n");

	init_leds();
	while (1) {
		stm32f7_delay(1000000);
		BSP_LED_Toggle(LED1);
		stm32f7_delay(1000000);
		BSP_LED_Off(LED1);
	}

	return 0;
}
