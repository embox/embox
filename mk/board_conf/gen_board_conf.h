#ifndef GEN_BOARD_CONF_H_
#define GEN_BOARD_CONF_H_

#include <stdint.h>
#include <stddef.h>

struct field_int {
	const char *name;
	const char *val;
};

struct field_reg_map {
	const char *name;
	const char *baseaddr;
	const char *len;
};

struct field_pin {
	const char *name;
	const char *port;
	const char *n;
	const char *af;
};

struct device_conf {
	const char *name;
	struct field_reg_map regs[16];
	struct field_int irqs[16];
	struct field_pin pins[64];
	struct field_int clocks[16];
	struct field_int dmas[16];
};

struct gpio_conf {
	int status;
	struct device_conf dev;
	int port_num;
	int port_width;
};

struct fpioa_conf {
	int status;
	struct device_conf dev;
	int port_num;
	int port_width;
};

struct clk_conf {
	int status;
	struct device_conf dev;
	struct field_int type[16];

};

struct dma_conf {
	int status;
	struct device_conf dev;
};

struct mmc_conf {
	int status;
	struct device_conf dev;
};

struct uart_conf {
	int status;
	const char *name;
	struct device_conf dev;
	unsigned int baudrate;
};

struct spi_conf {
	int status;
	const char *name;
	struct device_conf dev;
	int bits_per_word;
	int clk_div;
};

struct i2c_conf {
	int status;
	const char *name;
	struct device_conf dev;
};

struct eth_conf {
	int status;
	const char *name;
	struct device_conf dev;
};

struct conf_item {
	void *ptr;
	unsigned array_size;
};

struct pwm_conf {
	const char *name;
	struct field_int channel;
	struct field_int instance;
	struct field_int servo_low;
	struct field_int servo_high;
	struct device_conf dev;
};

struct led_conf {
	int status;
	const char *name;
	struct field_int port;
	struct field_int pin;
	struct field_int level; /* LED on */
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define UART_IDX    0
#define SPI_IDX     1
#define I2C_IDX     2
#define PWM_IDX     3
#define LED_IDX     4
#define GPIO_IDX    5
#define FPIOA_IDX   6
#define CLK_IDX     7
#define MMC_IDX     8
#define DMA_IDX     9
#define MAX_IDX     10


#define EXPORT_CONFIG(...) \
	struct conf_item board_config[MAX_IDX] = { \
		__VA_ARGS__ \
	};

#define UART(uarts) \
	[UART_IDX] = { \
		(void *) &(uarts)[0], \
		ARRAY_SIZE(uarts), \
	}

#define SPI(spis) \
	[SPI_IDX] = { \
		(void *) &(spis)[0], \
		ARRAY_SIZE(spis), \
	}

#define I2C(i2cs) \
	[I2C_IDX] = { \
		(void *) &(i2cs)[0], \
		ARRAY_SIZE(i2cs), \
	}

#define PWM(pwms) \
	[PWM_IDX] = { \
		(void *) &(pwms)[0], \
		ARRAY_SIZE(pwms), \
	}

#define LED(leds) \
	[LED_IDX] = { \
		(void *) &(leds)[0], \
		ARRAY_SIZE(leds), \
	}

#define GPIO(gpios) \
	[GPIO_IDX] = { \
		(void *) &(gpios)[0], \
		ARRAY_SIZE(gpios), \
	}

#define FPIOA(fpioas) \
	[FPIOA_IDX] = { \
		(void *) &(fpioas)[0], \
		ARRAY_SIZE(fpioas), \
	}

#define CLK(clks) \
	[CLK_IDX] = { \
		(void *) &(clks)[0], \
		ARRAY_SIZE(clks), \
	}

#define DMA(dmas) \
	[DMA_IDX] = { \
		(void *) &(dmas)[0], \
		ARRAY_SIZE(dmas), \
	}

#define MMC(mmcs) \
	[MMC_IDX] = { \
		(void *) &(mmcs)[0], \
		ARRAY_SIZE(mmcs), \
	}

#define CONFIG   void config()

#define DISABLED   0
#define ENABLED    1

#define MACRO_STRING(...) __MACRO_STRING(__VA_ARGS__)
#define __MACRO_STRING(...) # __VA_ARGS__

#define VAL(_name, _n) \
	{ .name = _name, .val = MACRO_STRING(_n) }

#define REGMAP(_name, _addr, _len) \
	{ .name = _name, .baseaddr = MACRO_STRING(_addr), .len = MACRO_STRING(_len) }

#define PIN(_name, _gpio, _n, _af) \
	{ .name = _name, .port = MACRO_STRING(_gpio), .n = MACRO_STRING(_n), \
		.af = MACRO_STRING(_af) }

#endif /* GEN_BOARD_CONF_H_ */
