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

#include <drivers/sensors/lis3mdl.h>

int main(int argc, char **argv) {
	printf("Testing lis3mdl sensor output\n");

	if (0 != lis3mdl_init()) {
		printf("Failed to init lis3mdl!\n");
		return -1;
	}

	while (!diag_kbhit()) {
		struct lis3mdl_s mf;
		lis3mdl_get_data(&mf);	// mgauss
		int Tx8 = lis3mdl_get_temp_x8();		// °C
		printf("  Magnetic field X: %d, Y: %d, Z: %d mgauss    Temperature: %d.%03d°C        \r",
			(mf.x*1000l+mf.sens/2)/mf.sens,
			(mf.y*1000l+mf.sens/2)/mf.sens,
			(mf.z*1000l+mf.sens/2)/mf.sens,
			Tx8/8, abs(Tx8*1000/8)%1000);
		fflush(stdout);
		usleep(500*1000);
	}
	printf("\n");

	return 0;
}
