/**
 * @file devfs_notify_stub.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 12.03.2020
 */

/* This stub is used only in old devfs to notify that new device was created */
struct dev_module;
void devfs_notify_new_module(struct dev_module *devmod) {
	/* Do nothing */
}

void devfs_notify_del_module(struct dev_module *devmod) {
	/* Do nothing */
}
