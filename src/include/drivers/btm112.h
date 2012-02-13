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

typedef int (*btm_bt_rx_handle_t)(int cnt, void *data);

CALLBACK_DECLARE(btm_bt_rx_handle_t, __bt_rx);

#endif
