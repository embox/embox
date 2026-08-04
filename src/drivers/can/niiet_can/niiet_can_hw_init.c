/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <stdint.h>

#include <drivers/clk/niiet_rcu.h>
#include <drivers/gpio.h>
#include <util/macro.h>

#include <config/board_config.h>

#define CONF_PIN_PREF CONF_CAN_PIN_

#define CAN_PORT(name) MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _PORT)
#define CAN_PIN(name)  MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _NR)
#define CAN_AF(name)   MACRO_CONCAT(MACRO_CONCAT(CONF_PIN_PREF, name), _AF)

void niiet_can_bconf_init(void) {
#if defined(CONF_CAN_PIN_NODE0_RX_PORT) && defined(CONF_CAN_PIN_NODE0_TX_PORT)
	gpio_setup_mode(CAN_PORT(NODE0_RX), (1 << CAN_PIN(NODE0_RX)),
	    GPIO_MODE_ALT_SET(CAN_AF(NODE0_RX)));
	gpio_setup_mode(CAN_PORT(NODE0_TX), (1 << CAN_PIN(NODE0_TX)),
	    GPIO_MODE_ALT_SET(CAN_AF(NODE0_TX)));
#endif /* NODE 0 */

#if defined(CONF_CAN_PIN_NODE1_RX_PORT) && defined(CONF_CAN_PIN_NODE1_TX_PORT)
	gpio_setup_mode(CAN_PORT(NODE1_RX), (1 << CAN_PIN(NODE1_RX)),
	    GPIO_MODE_ALT_SET(CAN_AF(NODE1_RX)));

	gpio_setup_mode(CAN_PORT(NODE1_TX), (1 << CAN_PIN(NODE1_TX)),
	    GPIO_MODE_ALT_SET(CAN_AF(NODE1_TX)));
#endif /* NODE1 */

	clk_enable(CONF_CAN_CLK_DEF);
}
