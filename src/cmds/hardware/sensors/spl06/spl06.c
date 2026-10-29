#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <drivers/sensors/spl06.h>

int main(int argc, char **argv) {
	printf("Testing spl06\n");

	if (0 != spl06_init()) {
		printf("Failed to init spl06!\n");
		return -1;
	}

	int temp = spl06_get_temperature();
	int press = spl06_get_pressure();

	printf("Temperature: %4d\n", temp);
	printf("Pressure: %4d\n", press);
}
