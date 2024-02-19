/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.02.24
 */

#ifndef DEVICE_CHAR_DEV_IMPL_H_
#define DEVICE_CHAR_DEV_IMPL_H_

#include <drivers/device.h>
#include <framework/mod/options.h>
#include <lib/libds/array.h>

#define MAX_CDEV_QUANTITY \
	OPTION_MODULE_GET(embox__device__char_dev_impl, NUMBER, dev_quantity)

#define CHAR_DEV_DEF(cdev_name, idesc_ops, priv)                           \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __char_device_registry); \
	ARRAY_SPREAD_ADD_NAMED(__char_device_registry,                         \
	    MACRO_CONCAT(__char_device_registry_ptr_, cdev_name),              \
	    {                                                                  \
	        .dev_id = DEVID_CDEV,                                          \
	        .name = MACRO_STRING(cdev_name),                               \
	        .dev_iops = idesc_ops,                                         \
	        .dev_priv = priv,                                              \
	    })

#endif /* DEVICE_CHAR_DEV_IMPL_H_ */
