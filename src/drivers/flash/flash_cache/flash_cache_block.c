/**
 * @file
 *
 * @author Anton Bondarev
 * @date   19 Oct 2023
 */

#include <stdint.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

uintptr_t flash_cache_addr(struct flash_dev *flashdev) {
	return FLASH_CACHE_GET(flashdev, unused_ram);
}


