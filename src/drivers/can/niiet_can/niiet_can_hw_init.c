/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#include <drivers/gpio.h>

#include <framework/mod/options.h>
#include <util/macro.h>
#include <config/board_config.h>

#include <drivers/clk/niiet_rcu.h>

#include "niiet_can.h"

#define CONF_PIN_PREF        CONF_CAN_PIN_

#define USB_PORT(name) \
			MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _PORT)
#define USB_PIN(name) \
			MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _NR)
#define USB_AF(name)  \
			MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _AF)

void niiet_can_bconf_init(struct niiet_can_dev_priv *priv) {

#if defined(CONF_CAN_PIN_NODE0_RX_PORT) && defined(CONF_CAN_PIN_NODE0_TX_PORT) 
	gpio_setup_mode(USB_PORT(NODE0_RX), (1 << USB_PIN(NODE0_RX)),
	    GPIO_MODE_ALT_SET(USB_AF(NODE0_RX)));

	gpio_setup_mode(USB_PORT(NODE0_TX), (1 << USB_PIN(NODE0_TX)),
	    GPIO_MODE_ALT_SET(USB_AF(NODE0_TX)));

#endif /* defined(CONF_CAN_PIN_NODE0_RX_PORT) && defined(CONF_CAN_PIN_NODE0_TX_PORT)  */

#if defined(CONF_CAN_PIN_NODE1_RX_PORT) && defined(CONF_CAN_PIN_NODE1_TX_PORT) 
	gpio_setup_mode(USB_PORT(NODE1_RX), (1 << USB_PIN(NODE1_RX)),
	    GPIO_MODE_ALT_SET(USB_AF(NODE1_RX)));

	gpio_setup_mode(USB_PORT(NODE1_TX), (1 << USB_PIN(NODE1_TX)),
	    GPIO_MODE_ALT_SET(USB_AF(NODE1_TX)));

#endif /* defined(CONF_CAN_PIN_NODE1_RX_PORT) && defined(CONF_CAN_PIN_NODE1_TX_PORT)  */

	priv->canmsg_regs = (struct niiet_canmsg_regs *)(uintptr_t)CONF_CAN_REGION_MSG_BASE;

	clk_enable(CONF_CAN_CLK_DEF);
}