/**
 * @file
 *
 * @date 02.02.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>

#include <drivers/sensors/l3g4200d.h>

int main(int argc, char *argv[]) {
	if (0 > l3g4200d_init()) {
		printf("Failed to init L3G4200D\n");
		return -1;
	}

	return 0;
}
