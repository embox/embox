/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2022-07-30
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/time/ktime.h>
#include <kernel/thread.h>

#include <framework/mod/options.h>

#include <feather/servo.h>

#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"

int main(int argc, char **argv) {
	int pos = 0;
	BSP_PB_Init(0, 0);
	servo_init();

        if (argc > 1) {
          pos = atoi(argv[1]);
	  servo_set(pos);
          return 0;
        }

	while (1) {
		if (BSP_PB_GetState(0)) {
			pos++;
                }
	        servo_set(pos % 100);
                ksleep(10);
	}

	return 0;
}
