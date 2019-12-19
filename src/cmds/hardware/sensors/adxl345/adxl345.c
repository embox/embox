/**
 * @file adxl345.c
 * @brief Print adxl345 data in the endless loop
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2019
 */

#include <stdint.h>
#include <stdio.h>

#include <drivers/sensors/adxl345.h>

int main(int argc, char **argv) {
	printf("Testing adxl345 accelerometer output\n");

	if (0 != adxl345_init()) {
		printf("Failed to init ADXL345!\n");
		return -1;
	}

	while (1) {
		int x = adxl345_get_x();
		int y = adxl345_get_y();
		int z = adxl345_get_z();

		printf("X: %4d    Y: %4d    Z: %4d\n", x, y, z);
	}
}
