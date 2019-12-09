/**
 * @file
 * @brief Kernel file abstraction
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef FS_FILE_OPERATION_H_
#define FS_FILE_OPERATION_H_

#include <stddef.h>

struct inode;
struct file_desc;
struct idesc;


/* NOTE ON FILE OPEN
 *
 * Basically,  in  regular  file  systems  file  open  driver  function  should
 * just  return  the same  idesc  that  was  passed as second  parameter.  This
 * feature  is  required for device-dependent operations, otherwise just return
 * the second argument.
 */

struct file_operations {
	struct idesc *(*open)(struct inode *node, struct idesc *file_desc);
	int    (*close)(struct file_desc *desc);
	size_t (*read)(struct file_desc *desc, void *buf, size_t size);
	size_t (*write)(struct file_desc *desc, void *buf, size_t size);
	int    (*ioctl)(struct file_desc *desc, int request, void *data);
};

#endif /* FS_FILE_OPERATION_H_ */
