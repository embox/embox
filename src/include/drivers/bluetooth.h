/**
 * @file
 * @brief Bluetooth driver
 *
 * @date 24.02.11
 * @author Anton Bondarev
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <types.h>

#include <embox/service/callback.h>

#define BT_DRV_MSG_CONNECTED 1
#define BT_DRV_MSG_READ 2
#define BT_DRV_MSG_DISCONNECTED 3

void bt_clear_arm7_cmd(void);
void bt_set_arm7_cmd(void);

typedef int(*bt_comm_handler_t)(int msg, uint8_t *buff);

extern void bluetooth_set_handler(bt_comm_handler_t handler);

extern size_t bluetooth_read(uint8_t *buff, size_t len);
extern size_t bluetooth_write(uint8_t *buff, size_t len);

CALLBACK_DECLARE(bluetooth_uart);

typedef int (*nxt_bt_rx_handle_t)(void);
extern void nxt_bt_set_rx_handle(nxt_bt_rx_handle_t handle);

typedef int (*nxt_bt_state_handle_t)(void);
extern void nxt_bt_set_state_handle(nxt_bt_state_handle_t handle);

extern void nxt_bluetooth_reset(void);



#endif /* BLUETOOTH_H_ */
