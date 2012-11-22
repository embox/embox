/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */


#ifndef BLOCK_DEV_H_
#define BLOCK_DEV_H_

#include <fs/file_desc.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>
#include <util/array.h>
#include <util/indexator.h>

#define IOCTL_GETBLKSIZE        1
#define IOCTL_GETDEVSIZE        2
#define IOCTL_GETGEOMETRY       3
#define IOCTL_REVALIDATE        4

#define NODEV                 (-1)
#define DEV_TYPE_STREAM         1
#define DEV_TYPE_BLOCK          2
#define DEV_TYPE_PACKET         3

typedef struct block_dev {
	dev_t id;
	node_t *dev_node;
	char name[MAX_LENGTH_FILE_NAME];
	struct block_dev_driver *driver;
	void *privdata;

	size_t size;
	struct block_dev_cache *cache;
} block_dev_t;

typedef struct block_dev_driver {
	char *name;

	int (*ioctl)(block_dev_t *dev, int cmd, void *args, size_t size);
	int (*read)(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
	int (*write)(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
} block_dev_driver_t;

typedef int (* block_dev_module_init_ft)(void *args);
typedef struct block_dev_module {
	const char * name;
	block_dev_driver_t *dev_drv;
	const block_dev_module_init_ft init;
} block_dev_module_t;

extern const block_dev_module_t __block_dev_registry[];

typedef struct block_dev_cache {
	blkno_t blkno;
	blkno_t lastblkno;
	char *data;
	int blksize;
	int blkfactor;
	int depth;
	char *pool;
	int buff_cntr;
} block_dev_cache_t;

extern struct block_dev *block_dev_create(char *name, void *driver, void *privdata);
extern block_dev_t *block_dev(void *dev_id);
extern dev_t block_dev_open(char *name);
extern block_dev_cache_t *block_dev_cache_init(void *dev_id, int blocks);
extern block_dev_cache_t *block_dev_cached_read(void *dev_id, blkno_t blkno);
extern int block_dev_read(void *dev_id, char *buffer, size_t count, blkno_t blkno);
extern int block_dev_write(void *dev_id, char *buffer, size_t count, blkno_t blkno);
extern int block_dev_ioctl(void *dev_id, int cmd, void *args, size_t size);
extern int block_dev_close(void *dev_id);
extern int block_dev_destroy(void *dev_id);
extern int block_dev_named(char *name, struct indexator *indexator);

#define EMBOX_BLOCK_DEV(name, block_dev_driver, init_func) \
		ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_driver, init_func})

#endif /* BLOCK_DEV_H_ */
