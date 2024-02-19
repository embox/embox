/**
 * @file
 * @brief Stub for init and register char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-09-10
 *
 * This stub is required to have FS and have no char devs
 * at the same time.
 */

#include <errno.h>
#include <stddef.h>

#include <drivers/char_dev.h>
#include <util/log.h>

int char_dev_init_all(void) {
	log_warning(">>> %s", __func__);
	return 0;
}

int char_dev_register(struct dev_module *cdev) {
	log_warning(">>> %s", __func__);
	return -ENOSUPP;
}

int char_dev_default_fstat(struct idesc *idesc, void *buff) {
	log_warning(">>> %s", __func__);
	return 0;
}

int char_dev_default_open(struct idesc *idesc, void *source) {
	log_warning(">>> %s", __func__);
	return 0;
}

void char_dev_default_close(struct idesc *idesc) {
	log_warning(">>> %s", __func__);
}

struct dev_module *idesc_to_dev_module(struct idesc *idesc) {
	log_warning(">>> %s", __func__);
	return NULL;
}
