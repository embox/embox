/**
 * @file emulator.c
 * @brief Simple flash emulator
 *
 * @date 22.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <mem/sysmalloc.h>

#include <util/log.h>
#include <util/err.h>

struct flash_emulator_priv {
	uint8_t *base;
	size_t block_sz;
	size_t size;
};

int flash_emu_init (void *arg) {
	/* Do nothing */
	return 0;
}

static size_t flash_emu_query(struct flash_dev *dev, void *data, size_t len) {
	/* Not implemented yet */
	return 0;
}

static int flash_emu_erase_block(struct flash_dev *dev, uint32_t block_base) {
	struct flash_emulator_priv *priv;
	uint8_t *mem;
	int len;

	assert(dev);

	priv = dev->privdata;
	assert(priv);

	len = priv->block_sz;
	mem = priv->base + priv->block_sz * block_base;

	assert(mem >= priv->base);
	assert(mem < priv->base + priv->size);
	assert(mem + len >= priv->base);
	assert(mem + len <= priv->base + priv->size);

	memset(mem, 0xFF, len);

	return 0;
}

static int flash_emu_program (struct flash_dev *dev, uint32_t base,
		const void* data, size_t len) {
	struct flash_emulator_priv *priv;
	uint8_t *mem;

	assert(dev);
	assert(data);

	priv = dev->privdata;
	assert(priv);

	mem = priv->base + base;

	assert(mem >= priv->base);
	assert(mem < priv->base + priv->size);
	assert(mem + len >= priv->base);
	assert(mem + len <= priv->base + priv->size);

	memcpy(mem, data, len);

	return 0;
}

static int flash_emu_read (struct flash_dev *dev,
		uint32_t base, void* data, size_t len) {
	struct flash_emulator_priv *priv;
	uint8_t *mem;

	assert(dev);
	assert(data);

	priv = dev->privdata;
	assert(priv);

	mem = priv->base + base;

	assert(mem >= priv->base);
	assert(mem < priv->base + priv->size);
	assert(mem + len >= priv->base);
	assert(mem + len <= priv->base + priv->size);

	memcpy(data, mem, len);

	return 0;
}

static struct flash_dev_drv flash_emu_drv = {
	flash_emu_init,
	flash_emu_query,
	flash_emu_erase_block,
	flash_emu_program,
	flash_emu_read
};

int flash_emu_dev_create(const char *name, int blocks, size_t block_size) {
	struct flash_dev *flash;
	struct flash_emulator_priv *priv;
	int err;

	flash = flash_create(name, blocks * block_size);
	if (flash == NULL) {
		return -1;
	}

	priv = malloc(sizeof(struct flash_emulator_priv));
	if (priv == NULL) {
		err = -ENOMEM;
		goto out_free_flash;
	}

	flash->privdata = priv;

	priv->size = blocks * block_size;
	priv->block_sz = block_size;
	priv->base = malloc(priv->size);
	if (priv->base == NULL) {
		err = -ENOMEM;
		goto out_free_priv;
	}

	memset(priv->base, 0xFF, priv->size);

	flash->num_block_infos = 1;
	flash->block_info[0].block_size = block_size;
	flash->block_info[0].blocks = blocks;

	flash->size = priv->size;

	flash->drv = &flash_emu_drv;

	return 0;

out_free_priv:
	free(priv);
out_free_flash:
	flash_delete(flash);

	log_error("Failed to create flash emulator error=%d", err);

	return err;
}
