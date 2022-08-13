/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#include <stm32f3_discovery_gyroscope.h>

#include <embox/unit.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(gyro_init);

#define PI 3.14159265358979f


static float gyro_offset[3];

void gyro_get(float *value) {
	BSP_GYRO_GetXYZ(value);
}

/* mrad/s */
void gyro_data_normalize(float *in, float *out) {
	for (int i = 0; i < 3; i++)
		out[i] = in[i] * 8.75f / 360 * 2 * PI;
}

/* mrad/s, without offset */
void gyro_data_obtain(float *out) {
	float gyro[3] = {0};
	gyro_get(gyro);
	gyro_data_normalize(gyro, out);

	for (int i = 0; i < 3; i++)
		out[i] -= gyro_offset[i];
}

void gyro_calculate_offset(void){
	float eps = 0.2;

	for (int  i= 0; i < 1000; i++) {
		float gyro[3] = {0};
		BSP_GYRO_GetXYZ(gyro);

		for (int j = 0; j < 3; j++)
			gyro_offset[j] = (1 - eps)*gyro_offset[j] + eps*gyro[j];
	}
	gyro_data_normalize(gyro_offset, gyro_offset);
}

static int gyro_init(void) {
	int res;

	res = BSP_GYRO_Init();
    if (res != HAL_OK) {
        printk("BSP_GYRO_Init failed, returned %d\n", res);
        return -1;
    }
    gyro_calculate_offset();

    return 0;
}
