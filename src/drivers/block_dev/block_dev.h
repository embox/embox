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
#include <stdint.h>
#include <sys/types.h>

#include <drivers/device.h>
#include <framework/mod/options.h>
#include <lib/libds/array.h>
#include <util/member.h>

#include <config/embox/driver/block_dev.h>

#define MAX_BDEV_QUANTITY \
	OPTION_MODULE_GET(embox__driver__block_dev, NUMBER, dev_quantity)

#define IOCTL_GETBLKSIZE  1
#define IOCTL_GETDEVSIZE  2
#define IOCTL_GETGEOMETRY 3
#define IOCTL_REVALIDATE  4

struct block_dev {
	struct dev_module dev_module;

	const struct block_dev_ops *driver;

	uint64_t size;
	size_t block_size;
	struct block_dev_cache *cache;

	/* partitions */
	uint64_t start_offset;
	struct block_dev *parent_bdev;
};

struct block_dev_ops {
	int (*bdo_ioctl)(struct block_dev *bdev, int cmd, void *args, size_t size);
	int (*bdo_read)(struct block_dev *bdev, char *buffer, size_t count,
	    blkno_t blkno);
	int (*bdo_write)(struct block_dev *bdev, char *buffer, size_t count,
	    blkno_t blkno);

	int (*bdo_probe)(struct block_dev *bdev, void *args);
};

struct block_dev_module {
	const char *name;
	const struct block_dev_ops *dev_drv;
};

struct block_dev_cache {
	blkno_t blkno;
	blkno_t lastblkno;
	char *data;
	size_t blksize;
	int blkfactor;
	int depth;
	char *pool;
	int buff_cntr;
};

struct indexator;

extern struct block_dev **get_bdev_tab(void);

extern struct block_dev *block_dev_create(const char *name,
    const struct block_dev_ops *driver, void *privdata);
extern struct block_dev *block_dev(void *bdev);

extern struct block_dev_cache *block_dev_cache_init(void *bdev, int blocks);
extern struct block_dev_cache *block_dev_cached_read(void *bdev, blkno_t blkno);
extern int block_dev_cache_free(void *dev);

extern int block_dev_read(void *bdev, char *buffer, size_t count,
    blkno_t blkno);
extern int block_dev_read_buffered(struct block_dev *bdev, char *buffer,
    size_t count, size_t offset);
extern int block_dev_write_buffered(struct block_dev *bdev, const char *buffer,
    size_t count, size_t offset);
extern int block_dev_write(void *bdev, const char *buffer, size_t count,
    blkno_t blkno);
extern int block_dev_ioctl(void *bdev, int cmd, void *args, size_t size);
extern int block_dev_close(void *bdev);
extern int block_dev_destroy(struct block_dev *bdev);
extern int block_dev_named(const char *name, struct indexator *indexator);
extern struct block_dev_module *block_dev_lookup(const char *name);
extern void block_dev_free(struct block_dev *dev);
extern struct block_dev *block_dev_find(const char *bd_name);

extern int block_dev_max_id(void);
extern struct block_dev *block_dev_by_id(int id);

extern struct dev_module *block_dev_to_device(struct block_dev *dev);

extern uint64_t block_dev_size(struct block_dev *dev);
extern size_t block_dev_block_size(struct block_dev *dev);
extern struct block_dev *block_dev_parent(struct block_dev *dev);
extern const char *block_dev_name(struct block_dev *dev);
extern dev_t block_dev_id(struct block_dev *dev);
extern void *block_dev_priv(struct block_dev *dev);

#define BLOCK_DEV_DRIVER_DEF(name, block_dev_ops)                              \
	ARRAY_SPREAD_DECLARE(const struct block_dev_module, __block_dev_registry); \
	ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_ops})

extern int block_devs_init(void);

static inline struct block_dev *dev_module_to_bdev(struct dev_module *devmod) {
	return (struct block_dev *)member_cast_out(devmod, struct block_dev,
	    dev_module);
}

#endif /* BLOCK_DEV_H_ */
