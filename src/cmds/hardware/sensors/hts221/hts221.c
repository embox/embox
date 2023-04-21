/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <drivers/diag.h>

#include <drivers/sensors/hts221.h>

int main(int argc, char **argv) {
	printf("Testing hts221 sensor output\n");

	if (0 != hts221_init()) {
		printf("Failed to init HTS221!\n");
		return -1;
	}

	while (!diag_kbhit()) {
		int h = hts221_get_humidity();		// %
		int t10 = hts221_get_temp_x10();	// °C
		printf("  Humidity: %d\%    Temperature: %d.%d°C        \r", h, t10/10, abs(t10)%10);
		fflush(stdout);
		usleep(500*1000);
	}
	printf("\n");

	return 0;
}
