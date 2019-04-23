/**
 * @file
 * @brief Meta-driver declarations
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-29
 */

#ifndef COMMON_DEVICE_H_
#define COMMON_DEVICE_H_

#include <config/embox/driver/common.h>
#include <framework/mod/options.h>
#include <fs/idesc.h>

#define DEV_NAME_LEN \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, device_name_len)

#define MAX_DEV_MODULE_COUNT \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, max_dev_module_count)

#define STATIC_DEVMOD_ID -1

struct idesc;
struct idesc_ops;

struct dev_module {
	int    dev_id;
	char   name[DEV_NAME_LEN];

	const struct idesc_ops *dev_iops;

	struct idesc *(*dev_open)  (struct dev_module *, void *);
	void 		  (*dev_close) (struct idesc *);

	void  *dev_priv;
};

extern struct dev_module *dev_module_create(
	const char *name,
	struct idesc * (*open)  (struct dev_module *, void *),
	void		   (*close) (struct idesc *),
	const struct idesc_ops *dev_iops,
	void *privdata
);

extern int dev_module_destroy(struct dev_module *dev);

#endif /* COMMON_DEVICE_H_ */
