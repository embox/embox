/**
 * @file
 *
 * @brief
 *
 * @date 24.02.2011
 * @author Anton Bondarev
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <types.h>

extern size_t nxt_bluetooth_read(uint8_t *buff, size_t len);

extern size_t nxt_bluetooth_write(uint8_t *buff, size_t len);

#endif /* BLUETOOTH_H_ */
