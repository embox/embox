/**
 * @file
 * @brief Bluetooth protocol stack.
 *
 * @date 24.02.11
 * @author Anton Bondarev
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <types.h>


extern size_t nxt_bluetooth_read(uint8_t *buff, size_t len);
extern size_t nxt_bluetooth_write(uint8_t *buff, size_t len);

typedef int(*bt_comm_handler_t)(uint8_t *buff);

extern void bluetooth_set_handler(bt_comm_handler_t handler);

extern void bluetooth_set_init_read(int bytes_num);

#endif /* BLUETOOTH_H_ */
