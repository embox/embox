/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */
#include <stdint.h>

#include <stm32f3_discovery_accelerometer.h>

#include <libsensors/acc.h>

#include <kernel/printk.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(acc_init);


static float acc_offset[3];

void acc_get(int16_t *value) {
	BSP_ACCELERO_GetXYZ(value);
}

/* mm/s^2 */
void acc_data_normalize(int16_t *in, float *out) {
	for (int i = 0; i < 3; i++)
		out[i] = (in[i] / 16) * g;
}

/* mm/s^2, without offset */
void acc_data_obtain(float *out) {
	int16_t acc[3] = {0};
	BSP_ACCELERO_GetXYZ(acc);
	acc_data_normalize(acc, out);

	for (int i = 0; i < 3; i++)
		out[i] -= acc_offset[i];
}

void acc_calculate_offset(void) {
	int16_t temp[3] = {0};
	float eps = 0.2;

	for (int i=0; i<1000; i++){
		int16_t acc[3] = {0};
		BSP_ACCELERO_GetXYZ(acc);

		for (int j = 0; j < 3; j++)
			temp[j] = (1 - eps) * temp[j] + eps*acc[j];
	}

	acc_data_normalize(temp, acc_offset);
	acc_offset[2] -= g*1000;
}

static int acc_init(void) {
	int res;

	res = BSP_ACCELERO_Init();
    if (res != HAL_OK) {
        printk("BSP_ACCLEERO_Init failed, returned %d\n", res);
        return -1;
    }

    acc_calculate_offset();

	return res;
}
