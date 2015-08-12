/**
 * @file
 * @brief BTM 112 high-level routines
 *
 * @date 15.11.11
 * @author Anton Kozlov
 */

#ifndef BTM112_H_
#define BTM112_H_

#include <util/callback.h>
#include <drivers/bluetooth/bluetooth.h>

typedef int (*btm_bt_rx_handle_t)(int cnt, void *data);

CALLBACK_DECLARE(btm_bt_rx_handle_t, __bt_rx);

#endif
