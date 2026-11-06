/**
 * @file
 * @brief Read MPU6500 device identification.
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */

#include <stdio.h>

#include <drivers/sensors/mpu6500.h>

int main(int argc, char **argv) {
	int ret = mpu6500_init();

	if (ret) {
		printf("MPU6500 identification failed: %d\n", ret);
		return ret;
	}
	printf("MPU6500 detected\n");
	return 0;
}
