/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <lib/libds/array.h>

#include <drivers/pwm.h>

ARRAY_SPREAD_DEF(const struct pwm_desc, __pwm_desc_registry);

ARRAY_SPREAD_DEF(struct pwm_device *, __pwm_device_registry);
