/**
 * @file
 * @brief
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 */

#ifndef FLASH_DEV_H_
#define FLASH_DEV_H_

#include <stdbool.h>
#include <defines/size_t.h>
#include <drivers/flash/flash.h>

struct flash_dev;

/* Structure of device driver */
struct flash_dev_drv {
  int     (*flash_init) (void *arg);
  size_t  (*flash_query) (struct flash_dev *dev, void * data, size_t len);
  int     (*flash_erase_block) (struct flash_dev *dev, uint32_t block_base);
  int     (*flash_program) (struct flash_dev *dev, uint32_t base, const void* data, size_t len);
  int     (*flash_read) (struct flash_dev *dev, uint32_t base, void* data, size_t len);
};

/* Structure of flash device private info*/
struct flash_dev {
	struct block_dev *bdev;
	struct flash_dev_drv *drv;
	uint32_t     flags;            /* Device characteristics */
	uint32_t     start;            /* First address */
	uint32_t     end;              /* Last address */
	uint32_t     num_block_infos;  /* Number of entries */
	flash_block_info_t    block_info;      /* Info about one block size */
	void 		 *privdata;

	bool    init;             /* Device has been initialised */
};

typedef int (* flash_dev_module_init_ft)(void *args);
typedef struct flash_dev_module {
	const char * name;
	struct flash_dev_drv *dev_drv;
	const flash_dev_module_init_ft init;
	void *arg;
} flash_dev_module_t;

#include <util/array.h>

#define EMBOX_FLASH_DEV(name, flash_dev_drv, init_func) \
	ARRAY_SPREAD_DECLARE(const flash_dev_module_t, __flash_dev_registry); \
	ARRAY_SPREAD_ADD(__flash_dev_registry, {name, flash_dev_drv, init_func})

extern int block_devs_init(void);

#endif /* FLASH_DEV_H_ */
