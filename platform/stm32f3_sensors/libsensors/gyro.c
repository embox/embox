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

void gyro_get(float *value) {
	BSP_GYRO_GetXYZ(value);
}

static int gyro_init(void) {
	int res;

	res = BSP_GYRO_Init();
    if (res != HAL_OK) {
        printk("BSP_GYRO_Init failed, returned %d\n", res);
        return -1;
    }

    return 0;
}
