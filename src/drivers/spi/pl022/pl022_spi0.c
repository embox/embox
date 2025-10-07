/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <stdint.h>
#include <string.h>
#include <drivers/spi.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#include "pl022_spi.h"

#define SPI_BUS_NUM        0

#define SPI_BASE_ADDR	  (uintptr_t)OPTION_GET(NUMBER,base_addr)

#define SPI_BUS_NAME      MACRO_CONCAT(spibus,SPI_BUS_NUM)

EMBOX_UNIT_INIT(pl022_spi0_init);

static struct pl022_spi pl022_spi0 = {
	.base_addr = SPI_BASE_ADDR,
};

#define USE_BOARD_CONF	  OPTION_GET(BOOLEAN,use_board_conf)

#if USE_BOARD_CONF == 1
#include <drivers/gpio.h>

#include <config/board_config.h>

#define CONF_SPI         MACRO_CONCAT(CONF_SPI,SPI_BUS_NUM)

#define SCLK_PORT        MACRO_CONCAT(CONF_SPI,_PIN_CLK_PORT)
#define SCLK_PIN         MACRO_CONCAT(CONF_SPI,_PIN_CLK_NR)
#define SCLK_AF          MACRO_CONCAT(CONF_SPI,_PIN_CLK_AF)

#define RX_PORT          MACRO_CONCAT(CONF_SPI,_PIN_RX_PORT)
#define RX_PIN           MACRO_CONCAT(CONF_SPI,_PIN_RX_NR)
#define RX_AF            MACRO_CONCAT(CONF_SPI,_PIN_RX_AF)

#define TX_PORT          MACRO_CONCAT(CONF_SPI,_PIN_TX_PORT)
#define TX_PIN           MACRO_CONCAT(CONF_SPI,_PIN_TX_NR)
#define TX_AF            MACRO_CONCAT(CONF_SPI,_PIN_TX_AF)

#define CLK_NAME         MACRO_CONCAT(CONF_SPI,_CLK_DEF_SPI)

extern int clk_enable(char *clk_name);

static const struct pin_description pl022_spi_pins[] = {
	{SCLK_PORT, SCLK_PIN, SCLK_AF},
	{RX_PORT, RX_PIN, RX_AF},
	{TX_PORT, TX_PIN, TX_AF},
};

static void hw_pins_config(struct spi_controller *spi_c) {
	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_pin),
			GPIO_MODE_OUT | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_func));

	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_pin),
			GPIO_MODE_OUT | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_func));

	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_pin),
			GPIO_MODE_IN | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_TX_IDX].pd_func));
}

#endif /* USE_BOARD_CONF */

static int pl022_spi0_init(void) {
	struct spi_controller *spi_c = spi_controller_by_id(SPI_BUS_NUM);

	pl022_spi0.spi_controller = spi_c;
	spi_c->spic_label = SPI_BASE_ADDR;
#if USE_BOARD_CONF == 1
	spi_c->spic_pins = pl022_spi_pins;
	clk_enable(CLK_NAME);
	hw_pins_config(spi_c);
#else
	spi_c->spic_pins = NULL;
#endif /* USE_BOARD_CONF */

	pl022_spi_init(&pl022_spi0);

	return 0;
}

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &pl022_spi_ops, &pl022_spi0, SPI_BUS_NUM);
