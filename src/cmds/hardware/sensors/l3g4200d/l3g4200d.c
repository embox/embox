/**
 * @file
 * @brief
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <drivers/sensors/l3g4200d.h>

int main(int argc, char **argv) {
	printf("Testing l3g4200d gyroscope output\n");

	if (0 != l3g4200d_init()) {
		printf("Failed to init L3G4200D!\n");
		return -1;
	}

	while (1) {
		int x = l3g4200d_get_angular_rate_x();
		int y = l3g4200d_get_angular_rate_y();
		int z = l3g4200d_get_angular_rate_z();

		printf("X: %4d    Y: %4d    Z: %4d\n", x, y, z);
	}
}
