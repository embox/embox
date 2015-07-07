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

void acc_get(int16_t *value) {
	BSP_ACCELERO_GetXYZ(value);
}

static int acc_init(void) {
	int res;

	res = BSP_ACCELERO_Init();
    if (res != HAL_OK) {
        printk("BSP_ACCLEERO_Init failed, returned %d\n", res);
        return -1;
    }

	return res;
}
