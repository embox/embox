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

#include <drivers/sensors/lps22hb.h>

int main(int argc, char **argv) {
	printf("Testing lps22hb sensor output\n");

	if (0 != lps22hb_init()) {
		printf("Failed to init LPS22HB!\n");
		return -1;
	}

	while (!diag_kbhit()) {
		int Px4096 = lps22hb_get_pressure_4096();	// hPa
		int Tx100 = lps22hb_get_temp_x100();		// °C
		printf("  Atmosphere pressure: %d mm Hg    Temperature: %d.%02d°C        \r",
			Px4096*3/16384, Tx100/100, abs(Tx100)%100);
		fflush(stdout);
		usleep(500*1000);
	}
	printf("\n");

	return 0;
}
