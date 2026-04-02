/**
 * @file
 * @brief
 *
 * @date 02.04.2026
 * @author Dmitry Pilyuk
 */

#include <stdio.h>
#include <unistd.h>

#include <drivers/sensors/mpu6000.h>

int main(int argc, char **argv) {
	printf("Testing MPU6000 gyroscope output\n");

	if (0 != mpu6000_init()) {
		printf("Failed to init MPU6000!\n");
		return -1;
	}
}
