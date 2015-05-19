/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    19.05.2015
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <stm32f3_discovery_gyroscope.h>

int main(int argc, char *argv[]) {
	BSP_GYRO_Init();

	return 0;
}
