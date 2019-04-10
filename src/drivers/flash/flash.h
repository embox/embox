/**
 * @file flash.h
 * @brief Interface for flash devices
 *
 * @date 20.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <framework/mod/options.h>
#include <config/embox/driver/flash/core.h>
#include <util/array.h>

struct flash_dev;

/* Structure of device driver */
struct flash_dev_drv {
	int	(*flash_init) (void *arg);
	size_t	(*flash_query) (struct flash_dev *dev, void * data, size_t len);
	int	(*flash_erase_block) (struct flash_dev *dev, uint32_t block_base);
	int	(*flash_program) (struct flash_dev *dev, uint32_t base,
	                          const void* data, size_t len);
	int	(*flash_read) (struct flash_dev *dev, uint32_t base,
	                       void* data, size_t len);
	int	(*flash_copy) (struct flash_dev *dev,
	                       uint32_t base_to, uint32_t base_from, size_t len);
};

typedef struct {
	uint32_t offset;
	size_t len;
	int flasherr;
	uint32_t err_address;
} flash_ioctl_erase_t;

typedef struct {
	size_t dev_size;
} flash_ioctl_devsize_t;

typedef struct {
	uint32_t dev_addr;
} flash_ioctl_devaddr_t;

typedef struct {
	size_t block_size;
} flash_ioctl_blocksize_t;

typedef struct flash_block_info {
	size_t           block_size;
	uint32_t         blocks;
} flash_block_info_t;

/* Structure of flash device info */
#define FLASH_BLOCK_INFO_MAX \
	OPTION_MODULE_GET(embox__driver__flash__core, NUMBER, block_info_max)
struct flash_dev {
	struct block_dev           *bdev;
	int                         idx;
	const struct flash_dev_drv *drv;
	uint32_t                    size; /* Bytes */
	uint32_t                    num_block_infos;
	flash_block_info_t          block_info[FLASH_BLOCK_INFO_MAX];
	void                       *privdata;
};

/* Handlers to check ranges and call device-specific funcions */
extern int flash_read(struct flash_dev *flashdev, unsigned long offset,
		void *buf, size_t len);
extern int flash_write(struct flash_dev *flashdev, unsigned long offset,
		const void *buf, size_t len);
extern int flash_erase(struct flash_dev *flashdev, uint32_t block);
extern int flash_copy(struct flash_dev *flashdev, uint32_t to,
		uint32_t from, size_t len);

typedef int (* flash_dev_module_init_ft)(void *args);
typedef struct flash_dev_module {
	const char *name;
	const struct flash_dev_drv *dev_drv;
	const flash_dev_module_init_ft init;
	void *arg;
} flash_dev_module_t;

#define FLASH_DEV_DEF(name, flash_dev_drv, init_func) \
	ARRAY_SPREAD_DECLARE(const flash_dev_module_t, __flash_dev_registry); \
	ARRAY_SPREAD_ADD(__flash_dev_registry, {name, flash_dev_drv, init_func})

/* ======== 0x600 FLASH ===============================
 * Get/Set configuration 'key' values for FLASH drivers
 */
#define FLASH_IOCTL_ERASE            0x600
#define FLASH_IOCTL_QUERY            0x601
#define FLASH_IOCTL_LOCK             0x602
#define FLASH_IOCTL_UNLOCK           0x603
#define FLASH_IOCTL_VERIFY           0x604
#define FLASH_IOCTL_DEVSIZE          0x605
#define FLASH_IOCTL_BLOCKSIZE        0x606
#define FLASH_IOCTL_DEVADDR          0x607

#define SET_CONFIG_FLASH_FIS_NAME         0x680

extern struct flash_dev *flash_create(const char *name, size_t size);
extern struct flash_dev *flash_by_id(int idx);
extern int flash_max_id(void);
extern int flash_delete(struct flash_dev *dev);

/* These two functions are for internal use and generally
 * should not be called from user space */
extern struct flash_dev *flash_alloc(void);
extern int flash_free(struct flash_dev *dev);
extern int flash_devs_init(void);

#endif /* FLASH_H_ */
