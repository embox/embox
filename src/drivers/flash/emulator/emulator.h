/**
 * @file emulator.h
 * @brief Access to emulator flash device
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.04.2019
 */

#ifndef FLASH_EMULATOR_H_
#define FLASH_EMULATOR_H_

#include <stdint.h>

extern int flash_emu_dev_create(const char *name, int blocks, size_t block_size);

#endif /* FLASH_EMULATOR_H_ */
