/**
 * @file dw_spi.c
 * @brief Designware SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 19.12.2019
 */
#include <errno.h>

#include <util/macro.h>

#include <drivers/common/memory.h>
#include <drivers/spi.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#include <drivers/gpio.h>

#include "dw_spi.h"

extern struct spi_controller_ops dw_spic_ops;
extern int dw_spi_init(struct dw_spi_priv *dw_spi, uintptr_t base_addr, int spi_nr);

#define DW_SPI_BASE               OPTION_GET(NUMBER, base_addr)

#define SPI_BUS_NUM               0

#define SPI_BUS_NAME              MACRO_CONCAT(dw_spi,SPI_BUS_NUM)
#define DW_SPI_CTRL_STRUCT_NAME   MACRO_CONCAT(dw_spi_priv,SPI_BUS_NUM)

static struct dw_spi_priv DW_SPI_CTRL_STRUCT_NAME;

PERIPH_MEMORY_DEFINE(SPI_BUS_NAME, DW_SPI_BASE, 0x100);

#define USE_BOARD_CONF	  OPTION_GET(BOOLEAN,use_board_conf)

#if USE_BOARD_CONF == 1
#include <drivers/gpio.h>

#include <config/board_config.h>

#define CONF_SPI         MACRO_CONCAT(CONF_SPI,SPI_BUS_NUM)

#define SCK_PORT        MACRO_CONCAT(CONF_SPI,_PIN_SCK_PORT)
#define SCK_PIN         MACRO_CONCAT(CONF_SPI,_PIN_SCK_NR)
#define SCK_AF          MACRO_CONCAT(CONF_SPI,_PIN_SCK_AF)

#define MISO_PORT          MACRO_CONCAT(CONF_SPI,_PIN_MISO_PORT)
#define MISO_PIN           MACRO_CONCAT(CONF_SPI,_PIN_MISO_NR)
#define MISO_AF            MACRO_CONCAT(CONF_SPI,_PIN_MISO_AF)

#define MOSI_PORT          MACRO_CONCAT(CONF_SPI,_PIN_MOSI_PORT)
#define MOSI_PIN           MACRO_CONCAT(CONF_SPI,_PIN_MOSI_NR)
#define MOSI_AF            MACRO_CONCAT(CONF_SPI,_PIN_MOSI_AF)

#define CLK_NAME         MACRO_CONCAT(CONF_SPI,_CLK_DEF_SPI)

extern int clk_enable(char *clk_name);

static const struct pin_description dw_spi_pins[] = {
	{SCK_PORT, SCK_PIN, SCK_AF},
	{MISO_PORT, MISO_PIN, MISO_AF},
	{MOSI_PORT, MOSI_PIN, MOSI_AF},
};

static void hw_pins_config(struct spi_controller *spi_c) {
	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_pin),
			GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_SCLK_IDX].pd_func));

	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_MISO_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_MISO_IDX].pd_pin),
			GPIO_MODE_IN | GPIO_MODE_IN_PULL_UP | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_MISO_IDX].pd_func));

	gpio_setup_mode(spi_c->spic_pins[SPIC_PIN_MOSI_IDX].pd_port,
			(1 << spi_c->spic_pins[SPIC_PIN_MOSI_IDX].pd_pin),
			GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_ALT_SET(spi_c->spic_pins[SPIC_PIN_MOSI_IDX].pd_func));
}

#endif /* USE_BOARD_CONF */

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &dw_spic_ops, &DW_SPI_CTRL_STRUCT_NAME, SPI_BUS_NUM, NULL);

EMBOX_UNIT_INIT(dw_spi_module_init);
static int dw_spi_module_init(void) {
	struct spi_controller *spi_c = spi_controller_by_id(SPI_BUS_NUM);

	spi_c->spic_label = DW_SPI_BASE;
#if USE_BOARD_CONF == 1
	spi_c->spic_pins = dw_spi_pins;
	clk_enable(CLK_NAME);
	hw_pins_config(spi_c);
#else
	spi_c->spic_pins = NULL;
#endif /* USE_BOARD_CONF */

	dw_spi_init(&DW_SPI_CTRL_STRUCT_NAME, DW_SPI_BASE, SPI_BUS_NUM);

	return 0;
}
