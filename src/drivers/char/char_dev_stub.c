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

#include <drivers/char_dev.h>

/**
 * @brief Stub
 *
 * @return Always -1
 */
int char_dev_init_all(void) {
	return -1;
}

/**
 * @brief Stub
 *
 * @param name Device name
 * @param ops Device file operations
 *
 * @return Always -1
 */
int char_dev_register(const char *name, const struct kfile_operations *ops) {
	return -1;
}
