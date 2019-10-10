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

struct file_operations;
typedef struct block_dev {
	dev_t id;
	char name[NAME_MAX + 1];
	void *dev_vfs_info;

	struct block_dev_driver *driver;
	void *privdata;

	size_t size;
	size_t block_size;
	struct block_dev_cache *cache;

	struct dev_module *dev_module;

	/* partitions */
	size_t start_offset;
	struct block_dev *parrent_bdev;
} block_dev_t;

typedef struct block_dev_driver {
	char *name;

	int (*ioctl)(struct block_dev *bdev, int cmd, void *args, size_t size);
	int (*read)(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
	int (*write)(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);

	int (*probe)(void *args);
} block_dev_driver_t;

typedef struct block_dev_module {
	const char * name;
	block_dev_driver_t *dev_drv;
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
struct dev_module;

extern struct block_dev **get_bdev_tab(void);

extern struct block_dev *block_dev_create(const char *name, void *driver, void *privdata);
extern struct block_dev *block_dev(void *bdev);

extern block_dev_cache_t *block_dev_cache_init(void *bdev, int blocks);
extern block_dev_cache_t *block_dev_cached_read(void *bdev, blkno_t blkno);
extern int block_dev_read(void *bdev, char *buffer, size_t count, blkno_t blkno);
extern int block_dev_read_buffered(struct block_dev *bdev, char *buffer, size_t count, size_t offset);
extern int block_dev_write_buffered(struct block_dev *bdev, const char *buffer, size_t count, size_t offset);
extern int block_dev_write(void *bdev, const char *buffer, size_t count, blkno_t blkno);
extern int block_dev_ioctl(void *bdev, int cmd, void *args, size_t size);
extern int block_dev_close(void *bdev);
extern int block_dev_destroy(void *bdev);
extern int block_dev_named(const char *name, struct indexator *indexator);
extern struct block_dev_module *block_dev_lookup(const char *name);
extern void block_dev_free(struct block_dev *dev);
extern struct block_dev *block_dev_create_common(const char *path, void *driver, void *privdata);
extern struct block_dev *block_dev_find(const char *bd_name);

#include <util/array.h>

#define BLOCK_DEV_DEF(name, block_dev_driver) \
	ARRAY_SPREAD_DECLARE(const struct block_dev_module, __block_dev_registry); \
	ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_driver})


extern int block_devs_init(void);

#include <drivers/device.h>
static inline struct block_dev *dev_module_to_bdev(struct dev_module *devmod) {
	return (struct block_dev *) devmod->dev_priv;
}

/* This part is actually just for dvfs */
struct dev_module;
extern int bdev_read_block(struct dev_module *devmod, void *buf, int blk);
extern int bdev_write_block(struct dev_module *devmod, void *buf, int blk);
extern int bdev_read_blocks(struct dev_module *devmod, void *buf, int blk, int count);
extern int bdev_write_blocks(struct dev_module *devmod, void *buf, int blk, int count);

#endif /* BLOCK_DEV_H_ */
