/**
 * @file
 *
 * @author Anton Bondarev
 * @date   19 Oct 2023
 */

#include <stdint.h>

#include <drivers/flash/flash.h>

uintptr_t flash_cache_addr(struct flash_dev *flashdev) {
	return flashdev->block_info[0].blocks - 1;
}


