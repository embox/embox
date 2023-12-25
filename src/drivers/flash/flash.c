/**
 * @brief Common interface for flash subsystem
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <errno.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <util/indexator.h>
#include <util/math.h>

#define MAX_FLASHDEV_QUANTITY OPTION_GET(NUMBER, dev_quantity)

POOL_DEF(flash_pool, struct flash_dev, MAX_FLASHDEV_QUANTITY);
INDEX_DEF(flash_idx, 0, MAX_FLASHDEV_QUANTITY);

static struct flash_dev *flashdev_tab[MAX_FLASHDEV_QUANTITY];

struct flash_dev *flash_alloc(void) {
	struct flash_dev *flash;
	int idx;

	if (NULL == (flash = pool_alloc(&flash_pool))) {
		return err_ptr(ENOMEM);
	}

	idx = index_alloc(&flash_idx, INDEX_MIN);
	if (idx == INDEX_NONE) {
		pool_free(&flash_pool, flash);
		return err_ptr(ENOMEM);
	}

	memset(flash, 0, sizeof(*flash));
	flash->idx = idx;

	flashdev_tab[idx] = flash;

	return flash;
}

int flash_free(struct flash_dev *dev) {
	assert(dev);

	index_free(&flash_idx, dev->idx);
	flashdev_tab[dev->idx] = 0;
	pool_free(&flash_pool, dev);

	return 0;
}

static int flash_initialized = 0;
struct flash_dev *flash_by_id(int idx) {
	if (idx < 0 || idx >= MAX_FLASHDEV_QUANTITY) {
		return NULL;
	}

	if (!flash_initialized) {
		flash_devs_init();
	}

	return flashdev_tab[idx];
}

int flash_max_id(void) {
	return MAX_FLASHDEV_QUANTITY;
}

ARRAY_SPREAD_DEF(const struct flash_dev_module, __flash_dev_registry);
int flash_devs_init(void) {
	int ret;
	const struct flash_dev_module *fdev_module;

	if (flash_initialized) {
		return 0;
	}

	flash_initialized = 1;

	/* TODO pass some device-specific data to init funcion? */
	array_spread_foreach_ptr(fdev_module, __flash_dev_registry) {
		if (fdev_module->dev_drv->flash_init != NULL) {
			ret = fdev_module->dev_drv->flash_init(NULL, NULL);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

/* Handlers to check ranges and call device-specific functions */
int flash_read(struct flash_dev *flashdev, unsigned long offset, void *buf,
    size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_read);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_read(flashdev, offset, buf, len);
}

int flash_write(struct flash_dev *flashdev, unsigned long offset,
    const void *buf, size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_program);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_program(flashdev, offset, buf, len);
}

int flash_erase(struct flash_dev *flashdev, uint32_t block) {
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_erase_block);

	return flashdev->drv->flash_erase_block(flashdev, block);
}

struct flash_dev *flash_by_bdev(struct block_dev *bdev) {
	return bdev->dev_module.dev_priv;
}

int flash_read_aligned(struct flash_dev *flashdev, unsigned long offset,
    void *buff, size_t len) {
	int i;
	char *b;
	uint32_t word32;
	int head;
	int word_size;

	assert(buff);

	if (flashdev->fld_aligned_word) {
		b = flashdev->fld_aligned_word;
		word_size = flashdev->fld_word_size;
	}
	else {
		b = (void *)&word32;
		word_size = sizeof(word32);
	}

	head = offset % word_size;

	if (head) {
		size_t head_cnt = min(len, word_size - head);

		offset -= head;
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b + head, head_cnt);

		if (len <= head_cnt) {
			return 0;
		}

		buff += head_cnt;
		offset += word_size;
		len -= head_cnt;
	}

	for (i = 0; len >= word_size; i++) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b, word_size);

		offset += word_size;
		buff += word_size;
		len -= word_size;
	}

	if (len > 0) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b, len);
	}

	return 0;
}

/* @brief Write non-aligned raw data to \b erased NAND flash
 * @param offset Start position on disk
 * @param buff   Source of the data
 * @param len    Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
int flash_write_aligned(struct flash_dev *flashdev, unsigned long offset,
    const void *buff, size_t len) {
	int i;
	char *b;
	uint32_t word32;
	int head;
	int word_size;

	assert(buff);

	if (flashdev->fld_aligned_word) {
		b = flashdev->fld_aligned_word;
		word_size = flashdev->fld_word_size;
	}
	else {
		b = (void *)&word32;
		word_size = sizeof(word32);
	}

	head = offset % word_size;

	if (head) {
		size_t head_write_cnt = min(len, word_size - head);

		offset -= head;
		flash_read(flashdev, offset, b, word_size);
		memcpy(b + head, buff, head_write_cnt);
		flash_write(flashdev, offset, b, word_size);

		if (len <= head_write_cnt) {
			return 0;
		}

		buff += head_write_cnt;
		offset += word_size;
		len -= head_write_cnt;
	}

	for (i = 0; len >= word_size; i++) {
		memcpy(b, buff, word_size);
		flash_write(flashdev, offset, b, word_size);

		offset += word_size;
		buff += word_size;
		len -= word_size;
	}

	if (len > 0) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(b, buff, len);
		flash_write(flashdev, offset, b, word_size);
	}

	return 0;
}

int flash_copy_aligned(struct flash_dev *flashdev, unsigned long to,
    unsigned long from, int len) {
	char b[32];

	while (len > 0) {
		int tmp_len;

		tmp_len = min(len, sizeof(b));

		if (0 > flash_read_aligned(flashdev, from, b, tmp_len)) {
			return -1;
		}
		if (0 > flash_write_aligned(flashdev, to, b, tmp_len)) {
			return -1;
		}

		len -= tmp_len;
		to += tmp_len;
		from += tmp_len;
	}

	return 0;
}

int flash_copy_block(struct flash_dev *flashdev, unsigned int to,
    unsigned long from) {
	uint32_t block_size;

	block_size = flashdev->block_info[0].block_size;

	flash_erase(flashdev, to);

	return flash_copy_aligned(flashdev, to * block_size, from * block_size,
	    block_size);
}
