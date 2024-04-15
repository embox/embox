/**
 * @file
 *
 * @brief
 *
 * @date 29.05.2012
 * @author Andrey Gazukin
 */

#include <fs/kfsop.h>

int format(const char *pathname, const char *fs_type) {
	return kformat(pathname, fs_type);
}
