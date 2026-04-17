/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <drivers/sensors/bmp280.h>

int main(int argc, char **argv) {
	printf("Testing bmp280\n");

	if (0 != bmp280_init()) {
		printf("Failed to init bmp280!\n");
		return -1;
	}

	int temp = bmp280_get_temperature();

	printf("TEMP: %4d\n", temp);
}
