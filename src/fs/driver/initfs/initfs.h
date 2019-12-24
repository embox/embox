/**
 * @file
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_FS_DRIVER_INITFS_INITFS_H_
#define SRC_FS_DRIVER_INITFS_INITFS_H_

#include <stddef.h>

struct initfs_file_info {
	int start_pos;
};

/**
* @brief Should be used as inode->i_data, but only for directories
*/
struct initfs_dir_info {
	int    start_pos;

	char  *path;
	size_t path_len;
	char  *name;
	size_t name_len;
};

#endif /* SRC_FS_DRIVER_INITFS_INITFS_H_ */
