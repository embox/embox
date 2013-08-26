/**
 * @file
 * @brief
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#include <err.h>
#include <embox/unit.h>
#include <fs/vfs.h>

#include <mem/page.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <util/indexator.h>
#include <util/binalign.h>

#include <embox/block_dev.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_dev.h>


#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)


POOL_DEF(flash_pool,struct flash_dev,MAX_DEV_QUANTITY);
INDEX_DEF(flash_idx,0,MAX_DEV_QUANTITY);

static int flashbdev_init(void *arg);
static int flashbdev_ioctl(struct block_dev *bdev, int kmd, void *buf, size_t size);
static int flashbdev_read(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int flashbdev_write(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);


block_dev_driver_t flashbdev_pio_driver = {
	"flash_drv",
	flashbdev_ioctl,
	flashbdev_read,
	flashbdev_write
};

static int flash_get_index(char *path) {
	char *dev_name;
	int idx;

	if(NULL == (dev_name = strstr(path, "mtd"))) {
		return -1;
	}
	dev_name += sizeof("mtd");

	if(!isdigit((int)dev_name[0])) {
		return -1;
	}

	sscanf(dev_name, "%d", &idx);

	return idx;
}

struct flash_dev *flash_create(char *path, size_t size) {
	struct flash_dev *flash;
	int idx;

	if (NULL == (flash = pool_alloc(&flash_pool))) {
		return err_ptr(ENOMEM);
	}

	if (0 > (idx = block_dev_named(path, &flash_idx))) {
		pool_free(&flash_pool, flash);
		return err_ptr(ENOENT);
	}

	flash->bdev = block_dev_create(path, &flashbdev_pio_driver, flash);
	if (NULL == flash->bdev) {
		index_free(&flash_idx, idx);
		pool_free(&flash_pool, flash);
		return err_ptr(EIO);
	}

	flash->bdev->size = size;

	return flash;
}

struct flash_dev *flash_get_param(char *path) {
	node_t *flash_node;
	struct nas *nas;
	struct node_fi *node_fi;

	if (NULL == (flash_node = vfs_lookup(NULL, path))) {
		return NULL;
	}
	nas = flash_node->nas;
	node_fi = nas->fi;
	return (struct flash_dev *) block_dev(node_fi->privdata)->privdata;
}

int flash_delete(const char *name) {
	node_t *flash_node;
	struct flash_dev *flash;
	struct nas *nas;
	struct node_fi *node_fi;
	int idx;

	if (NULL == (flash_node = vfs_lookup(NULL, name))) {
		return -1;
	}

	nas = flash_node->nas;
	node_fi = nas->fi;
	if (NULL != (flash = (struct flash_dev *)
							block_dev(node_fi->privdata)->privdata)) {
		if (-1 != (idx = flash_get_index((char *)name))) {
			index_free(&flash_idx, idx);
		}
		pool_free(&flash_pool, flash);
		block_dev_destroy (node_fi->privdata);
		vfs_del_leaf(flash_node);
	}
	return 0;
}


static int flashbdev_init(void *arg) {

	return 0;
}

static int flashbdev_read(block_dev_t *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	struct flash_dev *flash;
	uint32_t startpos, endpos;

	flash = (struct flash_dev *)bdev->privdata;

	flash = (struct flash_dev *) bdev->privdata;
	startpos = flash->start + (blkno * flash->block_info.block_size);
	endpos = startpos + count - 1;
	if ( startpos < flash->start ) {
		return -EINVAL;
	}
	if ( endpos > flash->end ) {
		return -EINVAL;
	}

	if((!flash->drv) || (!flash->drv->flash_read)) {
		return -EINVAL;
	}
	return flash->drv->flash_read(flash, startpos, buffer, count);
}


static int flashbdev_write(block_dev_t *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	struct flash_dev *flash;
	uint32_t startpos, endpos;

	flash = (struct flash_dev *)bdev->privdata;

	flash = (struct flash_dev *) bdev->privdata;
	startpos = flash->start + (blkno * flash->block_info.block_size);

	endpos = startpos + count - 1;
	if ( startpos < flash->start ) {
		return -EINVAL;
	}
	if ( endpos > flash->end ) {
		return -EINVAL;
	}

	if((!flash->drv) || (!flash->drv->flash_program)) {
		return -EINVAL;
	}

	return flash->drv->flash_program(flash, startpos, buffer, count);
}



/*
static bool flashiodev_init( struct cyg_devtab_entry *tab )
{
  int stat = cyg_flashbdev_init(NULL);
  cyg_ucount32 i;

  if (stat == CYG_FLASH_ERR_OK)
  {
      for (i=0; i<CYGNUM_IO_FLASH_BLOCK_DEVICES; i++)
      {
          CYG_ASSERT( tab->handlers == &cyg_io_flashdev_ops, "Unexpected handlers - not flashdev_ops" );
          flashiodev_table[i].handle.handlers = &cyg_io_flashdev_ops;
          flashiodev_table[i].handle.status = CYG_DEVTAB_STATUS_BLOCK;
          flashiodev_table[i].handle.priv = &flashiodev_table[i]; // link back
      } // for
      cyg_drv_mutex_init( &flashiodev_table_lock );
  } // if

  return (stat == CYG_FLASH_ERR_OK);
} // flashiodev_init()

static int flashiodev_bread( cyg_io_handle_t handle, void *buf, cyg_uint32 *len,
                  cyg_uint32 pos) {
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flash_dev *dev = (struct flash_dev *)tab->priv;

  uint32_t startpos = dev->start + pos;
  Cyg_ErrNo err = ENOERR;
  int flasherr;

  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flash_dev*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );

#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
  uint32_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;
#endif

  flasherr = cyg_flash_read( startpos,(void *)buf, *len, NULL );

  if ( flasherr != CYG_FLASH_ERR_OK )
    err = -EIO; // just something sane
  return err;
} // flashiodev_bread()

static int flashiodev_bwrite( cyg_io_handle_t handle, const void *buf, cyg_uint32 *len,
                   cyg_uint32 pos ) {
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flash_dev *dev = (struct flash_dev *)tab->priv;

  int err = ENOERR;
  int flasherr;
  uint32_t startpos = dev->start + pos;

  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flash_dev*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );

  // Unlike some other cases we _do_ do bounds checking on this all the time, because
  // the consequences of writing over the wrong bit of flash are so nasty.
  uint32_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;

  flasherr = cyg_flash_program( startpos, (void *)buf, *len, NULL );

  if ( flasherr != CYG_FLASH_ERR_OK )
    err = -EIO; // just something sane
  return err;
}
*/

static int flash_erase(struct flash_dev * dev, uint32_t flash_base,
                size_t len, uint32_t *err_address) {
	uint32_t block, end_addr;
	size_t erase_count;
	int stat;
	size_t block_size;

	if((!dev->drv) || (!dev->drv->flash_erase_block)) {
		return -EINVAL;
	}

	/* Check whether or not we are going past the end of this device, on
	 * to the next one. If so the next device will be handled by a
	 * recursive call later on.
	 */
	if (len > (dev->end + 1 - flash_base)) {
		end_addr = dev->end;
	} else {
		end_addr = flash_base + len - 1;
	}
	/* erase can only happen on a block boundary, so adjust for this */
	block = flash_base;
	erase_count   = (end_addr + 1) - block;
	block_size = dev->block_info.block_size;

	while (erase_count > 0) {

		if (erase_count < block_size) {
			erase_count = block_size;
		}

		stat = dev->drv->flash_erase_block(dev, block);

		if (0 != stat) {
			if (err_address) {
				*err_address = block;
			}
			break;
		}
		block       += block_size;
		erase_count -= block_size;
	}
	return stat;
}

static int decode_flash_cmd(int cmd) {
	switch(cmd) {

	case IOCTL_GETBLKSIZE:
		return GET_CONFIG_FLASH_BLOCKSIZE;

	case IOCTL_GETDEVSIZE:
		return GET_CONFIG_FLASH_DEVSIZE;

	default:
		return cmd;
	}
}

static int flashbdev_ioctl(struct block_dev *bdev, int cmd,
									void *buf, size_t size) {
	struct flash_dev *dev;
	flash_getconfig_erase_t *e;
	uint32_t startpos, endpos;
	flash_getconfig_devsize_t *ds;
	flash_getconfig_devaddr_t *da;
	flash_getconfig_blocksize_t *bs;

	dev = (struct flash_dev *)bdev->privdata;

	cmd = decode_flash_cmd(cmd);

	switch (cmd) {

	case GET_CONFIG_FLASH_ERASE:
		e = (flash_getconfig_erase_t *) buf;
		startpos = dev->start + e->offset;

		/* Unlike some other cases we _do_ do bounds checking on this all the time, because
		* the consequences of erasing the wrong bit of flash are so nasty.
		*/
		endpos = startpos + e->len - 1;
		if ( startpos < dev->start ) {
			return -EINVAL;
		}

		if ( endpos > dev->end ) {
			return -EINVAL;
		}

		e->flasherr = flash_erase(dev, startpos, e->len, &e->err_address);
		return ENOERR;

	case GET_CONFIG_FLASH_DEVSIZE:
		ds = (flash_getconfig_devsize_t *) buf;

		if(NULL == ds) {
			return (dev->end - dev->start + 1);
		}

		ds->dev_size = dev->end - dev->start + 1;

		return ENOERR;

	case GET_CONFIG_FLASH_DEVADDR:
		da = (flash_getconfig_devaddr_t *)buf;

		if(NULL == da) {
			return (dev->start);
		}

		da->dev_addr = dev->start;

		return ENOERR;

	case GET_CONFIG_FLASH_BLOCKSIZE:
		bs = (flash_getconfig_blocksize_t *)buf;

		if(NULL == bs) {
			return (dev->block_info.block_size);
		}

		bs->block_size = dev->block_info.block_size;
		return ENOERR;

	default:
		return -EINVAL;
	}
}

EMBOX_BLOCK_DEV("flash", &flashbdev_pio_driver, flashbdev_init);

ARRAY_SPREAD_DEF(const flash_dev_module_t, __flash_dev_registry);

int flash_devs_init(void) {
	int ret, i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__flash_dev_registry); ++i) {
		if (__flash_dev_registry[i].init != NULL) {
			ret = __flash_dev_registry[i].init(NULL);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}
