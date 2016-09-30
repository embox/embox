/**
 * @file
 * @brief Bluetooth driver
 *
 * @date 24.02.11
 * @author Anton Bondarev
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdint.h>
#include <stddef.h>

#include <util/callback.h>

#define BT_DRV_MSG_CONNECTED 1
#define BT_DRV_MSG_READ 2
#define BT_DRV_MSG_DISCONNECTED 3

typedef int(*bt_comm_handler_t)(int msg, uint8_t *buff);

extern void bluetooth_set_handler(bt_comm_handler_t handler);

extern size_t bluetooth_read(size_t len);
extern size_t bluetooth_write(uint8_t *buff, size_t len);

CALLBACK_DECLARE(bt_comm_handler_t, bluetooth_uart);

typedef int (*nxt_bt_rx_handle_t)(int len, void *data);
CALLBACK_DECLARE(nxt_bt_rx_handle_t, bt_rx);

typedef int (*nxt_bt_state_handle_t)(void);
CALLBACK_DECLARE(nxt_bt_state_handle_t, bt_state);

/*
 * Does hard reset. Usually need during boot, or unmaintable hw failure
 */
extern void bluetooth_hw_hard_reset(void);

/*
 * Soft reset is a call for hw to break session and listen to incoming
 * just like after hard reset
 */
extern void bluetooth_hw_soft_reset(void);

/*
 * One should call this as response on connection request
 */
void bluetooth_hw_accept_connect(void);

#endif /* BLUETOOTH_H_ */
