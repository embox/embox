/**
 * @file
 *
 * @brief
 *
 * @date 07.12.2011
 * @author Anton Bondarev
 */

#ifndef FS_DRIVER_REGISTRY_H_
#define FS_DRIVER_REGISTRY_H_

typedef struct fs_driver_head {
	struct list_head *next;
	struct list_head *prev;
	fs_drv_t *drv;
} fs_driver_head_t;


#endif /* FS_DRIVER_REGISTRY_H_ */
