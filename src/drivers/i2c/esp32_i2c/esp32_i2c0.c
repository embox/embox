/**
 * @file
 * @brief ESP32-C5 I2C0 bus instance.
 *
 * @date Oct 31, 2026
 * @author Dmitrii Piliuk
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/gpio.h>
#include <drivers/gpio/esp32_gpio.h>
#include <drivers/i2c/i2c_priv.h>
#include <drivers/pin_description.h>
#include <hal/reg.h>
#include <embox/unit.h>

#include <config/board_config.h>

#define ESP32_I2C_PCR_CONF      0x20
#define ESP32_I2C_PCR_CLK_EN    (1U << 0)
#define ESP32_I2C_PCR_RST_EN    (1U << 1)
#define ESP32_I2C_PCR_SCLK_CONF 0x24
#define ESP32_I2C_PCR_SCLK_EN   (1U << 22)

extern const struct i2c_bus_ops esp32_i2c_ops;

/* AF contains the raw matrix signal ID for both input and output. */
static const struct pin_description esp32_i2c0_pins[] = {
	{CONF_I2C0_PIN_SCL_PORT, CONF_I2C0_PIN_SCL_NR, CONF_I2C0_PIN_SCL_AF},
	{CONF_I2C0_PIN_SDA_PORT, CONF_I2C0_PIN_SDA_NR, CONF_I2C0_PIN_SDA_AF},
};

static int esp32_i2c0_pin_init(const struct pin_description *pin,
    uint32_t mode) {
	int ret;

	if (pin->pd_pin < 0
	    || (unsigned int)pin->pd_pin >= ESP32_GPIO_PIN_COUNT
	    || pin->pd_func < 0
	    || (unsigned int)pin->pd_func >= ESP32_GPIO_SIGNAL_COUNT) {
		return -EINVAL;
	}

	/* Preload the released level before enabling open-drain output. */
	gpio_set(pin->pd_port, 1U << pin->pd_pin, GPIO_PIN_HIGH);
	ret = gpio_setup_mode(pin->pd_port, 1U << pin->pd_pin,
	    mode | GPIO_MODE_ALT_SET(ESP32_IO_MUX_FUNC_GPIO));
	if (ret) {
		return ret;
	}
	ret = gpio_setup_route(pin->pd_port, pin->pd_pin,
	    pin->pd_func | GPIO_ROUTE_INPUT);
	if (ret) {
		return ret;
	}

	return gpio_setup_route(pin->pd_port, pin->pd_pin,
	    pin->pd_func | GPIO_ROUTE_OUTPUT);
}

static int esp32_i2c0_hw_init(void) {
	uintptr_t pcr_base = CONF_I2C0_REGION_PCR;
	int ret;

	REG32_ORIN(pcr_base + ESP32_I2C_PCR_CONF, ESP32_I2C_PCR_CLK_EN);
	REG32_ORIN(pcr_base + ESP32_I2C_PCR_CONF, ESP32_I2C_PCR_RST_EN);
	REG32_CLEAR(pcr_base + ESP32_I2C_PCR_CONF, ESP32_I2C_PCR_RST_EN);
	/* XTAL source, no fractional or integer division. SCL timing is
	 * configured separately by the controller driver.
	 */
	REG32_STORE(pcr_base + ESP32_I2C_PCR_SCLK_CONF, 0);
	REG32_STORE(pcr_base + ESP32_I2C_PCR_SCLK_CONF,
	    ESP32_I2C_PCR_SCLK_EN);

	ret = esp32_i2c0_pin_init(&esp32_i2c0_pins[I2C_BUS_PIN_SCL],
	    CONF_I2C0_PIN_SCL_MODE);
	if (ret) {
		return ret;
	}

	return esp32_i2c0_pin_init(&esp32_i2c0_pins[I2C_BUS_PIN_SDA],
	    CONF_I2C0_PIN_SDA_MODE);
}

static const struct i2c_bus i2c_bus0 = {
	.i2cb_priv = NULL,
	.i2cb_ops = &esp32_i2c_ops,
	.i2cb_id = 0,
	.i2cb_pins = esp32_i2c0_pins,
	.i2cb_label = CONF_I2C0_REGION_BASE,
};

static int esp32_i2c0_init(void) {
	int ret;

	ret = esp32_i2c0_hw_init();
	if (ret) {
		return ret;
	}

	/* Registration calls the controller driver's i2c_init callback. */
	return i2c_bus_register(&i2c_bus0);
}

EMBOX_UNIT_INIT(esp32_i2c0_init);
