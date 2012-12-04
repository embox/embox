/*
 * @file
 *
 * @date Nov 28, 2012
 * @author: Anton Bondarev
 */

#include <embox/device.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const device_module_t, __device_registry);

int char_devs_init(void) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__device_registry); i++) {
		if(NULL != __device_registry[i].init) {
			__device_registry[i].init();
		}
	}
	return i;
}
