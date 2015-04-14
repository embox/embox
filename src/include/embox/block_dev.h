/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */


#ifndef BLOCK_DEV_H_
#define BLOCK_DEV_H_

#include <limits.h>
#include <sys/types.h>

#define IOCTL_GETBLKSIZE        1
#define IOCTL_GETDEVSIZE        2
#define IOCTL_GETGEOMETRY       3
#define IOCTL_REVALIDATE        4

#define NODEV                 (-1)
#define DEV_TYPE_STREAM         1
#define DEV_TYPE_BLOCK          2
#define DEV_TYPE_PACKET         3

#define SECTOR_SIZE		512		/* sector size in bytes */

typedef struct block_dev {
	dev_t id;
	char name[NAME_MAX];
	void *dev_vfs_info;

	struct block_dev_driver *driver;
	void *privdata;

	size_t size;
	struct block_dev_cache *cache;
} block_dev_t;

typedef struct block_dev_driver {
	char *name;

	int (*ioctl)(block_dev_t *bdev, int cmd, void *args, size_t size);
	int (*read)(block_dev_t *bdev, char *buffer, size_t count, blkno_t blkno);
	int (*write)(block_dev_t *bdev, char *buffer, size_t count, blkno_t blkno);
} block_dev_driver_t;

typedef int (* block_dev_module_init_ft)(void *args);
typedef struct block_dev_module {
	const char * name;
	block_dev_driver_t *dev_drv;
	const block_dev_module_init_ft init;
} block_dev_module_t;


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


struct indexator;
struct block_dev;

extern struct block_dev *block_dev_create(char *name, void *driver, void *privdata);
extern struct block_dev *block_dev(void *bdev);
extern dev_t block_dev_open(char *name);
extern block_dev_cache_t *block_dev_cache_init(void *bdev, int blocks);
extern block_dev_cache_t *block_dev_cached_read(void *bdev, blkno_t blkno);
extern int block_dev_read(void *bdev, char *buffer, size_t count, blkno_t blkno);
extern int block_dev_read_buffered(block_dev_t *bdev, char *buffer, size_t count, size_t offset);
extern int block_dev_write_buffered(block_dev_t *bdev, const char *buffer, size_t count, size_t offset);
extern int block_dev_write(void *bdev, const char *buffer, size_t count, blkno_t blkno);
extern int block_dev_ioctl(void *bdev, int cmd, void *args, size_t size);
extern int block_dev_close(void *bdev);
extern int block_dev_destroy(void *bdev);
extern int block_dev_named(char *name, struct indexator *indexator);
extern int block_dev_cache_free(void *dev);
extern block_dev_module_t *block_dev_lookup(const char *name);

#include <util/array.h>

#define EMBOX_BLOCK_DEV(name, block_dev_driver, init_func) \
	ARRAY_SPREAD_DECLARE(const block_dev_module_t, __block_dev_registry); \
	ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_driver, init_func})


extern int block_devs_init(void);

#endif /* BLOCK_DEV_H_ */
