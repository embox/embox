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

#include <drivers/sensors/lsm6dsl.h>

int main(int argc, char **argv) {
	printf("Testing lsm6dsl sensor output\n");

	if (0 != lsm6dsl_init()) {
		printf("Failed to init lsm6dsl!\n");
		return -1;
	}

	printf("         Gyroscope       Accelerometer     Temp    Time\n");
	printf("       X     Y     Z     X     Y     Z      °C       s \n");
	      /*  ±XXXXX±XXXXX±XXXXX±XXXXX±XXXXX±XXXXX   ±XX.XX   XXX.X */
	while (!diag_kbhit()) {
		struct lsm6dsl_s d;
		lsm6dsl_get_data(&d);
		int Tx256 = lsm6dsl_get_temp_x256();
		int ts = lsm6dsl_get_timestamp_us()/100000;
		printf("  %6d%6d%6d%6d%6d%6d   %3d.%02d   %3d.%1d   \r",
			d.g_x, d.g_y, d.g_z,
			d.xl_x, d.xl_y, d.xl_z,
			Tx256/256, abs(Tx256*100/256)%100,
			ts/10, ts%10);
		fflush(stdout);
		usleep(50*1000);
	}
	printf("\n");

	return 0;
}
