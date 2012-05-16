/**
 * @file
 *
 * @date 29.06.09
 * @author Anton Bondarev
 */

#ifndef FS_ROOTFS_H_
#define FS_ROOTFS_H_

#include <types.h>
#include <fs/node.h>

#define FILE_MODE_RO    0x1 /*read only     */
#define FILE_MODE_WO    0x2 /*write only    */
#define FILE_MODE_XO    0x4 /*exec only     */
#define FILE_MODE_RW    0x3 /*read write    */
#define FILE_MODE_RX    0x5 /*read execute  */
#define FILE_MODE_WX    0x6 /*write execute */
#define FILE_MODE_RWX   0x7 /*all           */

typedef struct _FILE_INFO {
	char file_name[CONFIG_MAX_LENGTH_FILE_NAME];
	unsigned int size_in_bytes;
	unsigned int size_on_disk;
	unsigned int mode;
} FILE_INFO;

typedef FILE_INFO *(*FS_FILE_ITERATOR)(FILE_INFO *file_info);

typedef FS_FILE_ITERATOR (*FS_GETFILELISTITERATOR_FUNC) (void);

//#include <file_op.h>

extern node_t *root_fs_get_node(void);

#endif /* FS_ROOTFS_H_ */
