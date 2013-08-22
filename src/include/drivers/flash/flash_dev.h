/**
 * @file
 * @brief
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 */

#ifndef FLASH_DEV_H_
#define FLASH_DEV_H_

struct flash_dev;

/* Structure of device driver */
struct flash_dev_drv {
  int     (*flash_init) (struct flash_dev *dev);
  size_t  (*flash_query) (struct flash_dev *dev, void * data, size_t len);
  int     (*flash_erase_block) (struct flash_dev *dev, uint32_t block_base);
  int     (*flash_program) (struct flash_dev *dev, uint32_t base, const void* data, size_t len);
  int     (*flash_read) (struct flash_dev *dev, uint32_t base, void* data, size_t len);
};

/* Structure of flash device */
struct flash_dev {
	struct block_dev *bdev;
	struct flash_dev_drv *drv;
	uint32_t     flags;            /* Device characteristics */
	uint32_t     start;            /* First address */
	uint32_t     end;              /* Last address */
	uint32_t     num_block_infos;  /* Number of entries */
	const flash_block_info_t    block_info;      /* Info about one block size */

	bool    init;             /* Device has been initialised */
};

#endif /* FLASH_DEV_H_ */
