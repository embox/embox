/**
 * @file nfs.h
 * @brief
 *
 * @date 14.05.2012
 * @author Andrey Gazukin
 */


#ifndef NFS_H_
#define NFS_H_

#include <stdint.h>
#include <fs/fat.h>
#include <fs/ramdisk.h>

#define MAX_FILE_QUANTITY  64

typedef struct nfs_fs_description {
	ramdisk_params_t * p_device;
	char root_name[CONFIG_MAX_LENGTH_FILE_NAME];
	vol_info_t vi;
} nfs_fs_description_t;

typedef struct _nfs_file_description {
	file_info_t fi;
	nfs_fs_description_t *p_fs_dsc;
} nfs_file_description_t;

#endif /* NFS_H_ */
