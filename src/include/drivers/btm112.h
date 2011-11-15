/**
 * @file
 * @brief
 *
 * @date
 * @author
 */

#ifndef BTM112_H_
#define BTM112_H_

#include <types.h>

#include <util/callback.h>
#include <drivers/bluetooth.h>

extern uint8_t *btm_bt_read_buff;
extern int btm_bt_read_len;

CALLBACK_DECLARE(nxt_bt_rx_handle_t, __bt_rx);

#endif
