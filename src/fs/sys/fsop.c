/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */


#include <fs/kfsop.h>


int mount(char *dev,  char *dir, char *fs_type) {
	return kmount(dev, dir, fs_type);
}

int format(const char *pathname, const char *fs_type) {
	return kformat(pathname, fs_type);
}

int umount(char *dir) {
	return kumount(dir);
}


