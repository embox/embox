/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */


#ifndef BLOCK_DEV_H_
#define BLOCK_DEV_H_

#include <kernel/file.h>
#include <fs/file_desc.h>

#include <util/array.h>

typedef size_t (*devop_read_block)(void *dev, char *buff,
		uint32_t lba, uint32_t sect_count);
typedef size_t (*devop_write_block)(void *dev, char *buff,
		uint32_t lba, uint32_t sect_count);
typedef int (*devop_ioctl)(void *dev);
typedef int (*devop_flush)(void *dev);

typedef struct block_dev_operations {
	devop_read_block  blk_read;
	devop_write_block blk_write;
	devop_ioctl ioctl;
	devop_flush flush;
} block_dev_operations_t;

typedef struct block_dev_module {
	const char * name;
	block_dev_operations_t *dev_ops;
} block_dev_module_t;

typedef struct dev_image {
	node_t *dev_node;
	size_t size;
} dev_image_t;

extern const block_dev_module_t __block_dev_registry[];
extern block_dev_module_t *block_dev_find(char *name);

#define EMBOX_BLOCK_DEV(name, block_dev_operations) \
	ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_operations})


#endif /* BLOCK_DEV_H_ */
