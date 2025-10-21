#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gen_board_conf.h"

/* It's conf/board.conf.h */
#include <board.conf.h>

static int gen_dev_enabled(const char *dev_name) {
	char buf[128];
	char def[64];

	sprintf(def, "#define CONF_%s_ENABLED", dev_name);
	sprintf(buf, "%-50s %d", def, 1);
	printf("%s\n", buf);
}

static int gen_prop_ival(const char *dev_name, const char *pr_name, int val) {
	char buf[128];
	char def[64];

	sprintf(def, "#define CONF_%s_%s", dev_name, pr_name);
	sprintf(buf, "%-50s %d", def, val);
	printf("%s\n", buf);
}

static int gen_field_int(const char *dev_name, const char *prop_name,
    const struct field_int *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	if (!strcmp(f->name, "")) {
		sprintf(def, "#define CONF_%s_%s", dev_name, prop_name);
	}
	else {
		if (prop_name != NULL && strlen(prop_name) != 0) {
			sprintf(def, "#define CONF_%s_%s_%s", dev_name, prop_name, f->name);
		}
		else {
			sprintf(def, "#define CONF_%s_%s", dev_name, f->name);
		}
	}
	sprintf(buf, "%-50s %s", def, f->val);
	printf("%s\n", buf);

	return 0;
}

static int gen_field_reg_map(const char *dev_name, const char *prop_name,
    const struct field_reg_map *f) {
	char buf[128];
	char def[64];
	char def_len[64];

	if (!f->name) {
		return -1;
	}

	if (!strcmp(f->name, "")) {
		sprintf(def, "#define CONF_%s_%s", dev_name, prop_name);
		sprintf(def_len, "#define CONF_%s_%s_LEN", dev_name, prop_name);
	}
	else {
		if (prop_name != NULL && strlen(prop_name) != 0) {
			sprintf(def, "#define CONF_%s_%s_%s", dev_name, prop_name, f->name);
			sprintf(def_len, "#define CONF_%s_%s_%s_LEN", dev_name, prop_name,
			    f->name);
		}
		else {
			sprintf(def, "#define CONF_%s_%s", dev_name, f->name);
			sprintf(def_len, "#define CONF_%s_%s_LEN", dev_name, f->name);
		}
	}
	sprintf(buf, "%-50s %s", def, f->baseaddr);
	printf("%s\n", buf);
	sprintf(buf, "%-50s %s", def_len, f->len);
	printf("%s\n", buf);

	return 0;
}
static int gen_field_func(const char *dev_name, const char *prop_name,
    const struct field_int *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	if (!strcmp(f->name, "")) {
		sprintf(def, "#define CONF_%s_%s()", dev_name, prop_name);
	}
	else {
		sprintf(def, "#define CONF_%s_%s_%s()", dev_name, prop_name, f->name);
	}
	sprintf(buf, "%-50s %s", def, f->val);
	printf("%s\n", buf);

	return 0;
}

static int gen_field_pin(const char *dev_name, const char *prop_name,
    const struct field_pin *f) {
	char buf[128];
	char def[64];

	if (!f->name) {
		return -1;
	}

	sprintf(def, "#define CONF_%s_%s_%s_PORT", dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->port);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_%s_%s_NR", dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->n);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_%s_%s_AF", dev_name, prop_name, f->name);
	sprintf(buf, "%-50s %s", def, f->af);
	printf("%s\n", buf);

	return 0;
}

static int gen_field_gpio_out(const char *dev_name, const char *port,
    const char *pin, const char *level) {
	char buf[128];
	char def[64];

	sprintf(def, "#define CONF_%s_GPIO_PORT", dev_name);
	sprintf(buf, "%-50s %s", def, port);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_GPIO_PIN", dev_name);
	sprintf(buf, "%-50s %s", def, pin);
	printf("%s\n", buf);

	sprintf(def, "#define CONF_%s_GPIO_LEVEL", dev_name);
	sprintf(buf, "%-50s %s", def, level);
	printf("%s\n", buf);

	return 0;
}

static int gen_device_conf(const struct device_conf *dev) {
	int j;

	gen_dev_enabled(dev->name);

	for (j = 0; j < ARRAY_SIZE(dev->regs); j++) {
		if (gen_field_reg_map(dev->name, "REGION", &dev->regs[j]) != 0) {
			break;
		}
	}

	for (j = 0; j < ARRAY_SIZE(dev->irqs); j++) {
		if (gen_field_int(dev->name, "IRQ", &dev->irqs[j]) != 0) {
			break;
		}
	}

	for (j = 0; j < ARRAY_SIZE(dev->pins); j++) {
		if (gen_field_pin(dev->name, "PIN", &dev->pins[j]) != 0) {
			break;
		}
	}

	for (j = 0; j < ARRAY_SIZE(dev->clocks); j++) {
		if (gen_field_func(dev->name, "CLK_ENABLE", &dev->clocks[j]) != 0) {
			break;
		}
		if (gen_field_int(dev->name, "CLK_DEF", &dev->clocks[j]) != 0) {
			break;
		}
	}

	for (j = 0; j < ARRAY_SIZE(dev->dmas); j++) {
		if (gen_field_int(dev->name, "DMA", &dev->dmas[j]) != 0) {
			break;
		}
	}

	for (j = 0; j < ARRAY_SIZE(dev->misc); j++) {
		if (gen_field_int(dev->name, "MISC", &dev->misc[j]) != 0) {
			break;
		}
	}

	return 0;
}

int main() {
	int i, j;
	struct conf_item *uart_conf = &board_config[UART_IDX];
	const struct uart_conf *uart;
	struct conf_item *spi_conf = &board_config[SPI_IDX];
	const struct spi_conf *spi;
	struct conf_item *i2c_conf = &board_config[I2C_IDX];
	const struct i2c_conf *i2c;
	struct conf_item *pwm_conf = &board_config[PWM_IDX];
	const struct pwm_conf *pwm;
	struct conf_item *led_conf = &board_config[LED_IDX];
	const struct led_conf *led;
	struct conf_item *gpio_conf = &board_config[GPIO_IDX];
	const struct gpio_conf *gpio;
	struct conf_item *fpioa_conf = &board_config[FPIOA_IDX];
	const struct fpioa_conf *fpioa;
	struct conf_item *clk_conf = &board_config[CLK_IDX];
	const struct clk_conf *clk;
	struct conf_item *mmc_conf = &board_config[MMC_IDX];
	const struct mmc_conf *mmc;
	struct conf_item *lcd_conf = &board_config[LCD_IDX];
	const struct lcd_conf *lcd;

	config();

	printf("#ifndef BOARD_CONFIG_H_\n");
	printf("#define BOARD_CONFIG_H_\n\n");

	/* CLK */
	for (i = 0; i < clk_conf->array_size; i++) {
		clk = &((const struct clk_conf *)clk_conf->ptr)[i];

		if (clk->status != ENABLED) {
			continue;
		}

		gen_device_conf(&clk->dev);

		for (j = 0; j < ARRAY_SIZE(clk->type); j++) {
			gen_field_int(clk->dev.name, "TYPE", &clk->type[j]);
		}

		printf("\n");
	}

	/* GPIO */
	for (i = 0; i < gpio_conf->array_size; i++) {
		gpio = &((const struct gpio_conf *)gpio_conf->ptr)[i];

		if (gpio->status != ENABLED) {
			continue;
		}

		gen_device_conf(&gpio->dev);

		gen_prop_ival(gpio->dev.name, "NUM", gpio->port_num);
		gen_prop_ival(gpio->dev.name, "WIDTH", gpio->port_width);

		printf("\n");
	}

	/* FPIOA */
	for (i = 0; i < fpioa_conf->array_size; i++) {
		fpioa = &((const struct fpioa_conf *)fpioa_conf->ptr)[i];

		if (fpioa->status != ENABLED) {
			continue;
		}

		gen_device_conf(&fpioa->dev);

		gen_prop_ival(fpioa->dev.name, "NUM", fpioa->port_num);
		gen_prop_ival(fpioa->dev.name, "WIDTH", fpioa->port_width);

		printf("\n");
	}

	/* UART */
	for (i = 0; i < uart_conf->array_size; i++) {
		uart = &((const struct uart_conf *)uart_conf->ptr)[i];

		if (uart->status != ENABLED) {
			continue;
		}

		gen_device_conf(&uart->dev);

		printf("\n");
	}

	/* SPI */
	for (i = 0; i < spi_conf->array_size; i++) {
		spi = &((const struct spi_conf *)spi_conf->ptr)[i];

		if (spi->status != ENABLED) {
			continue;
		}

		gen_device_conf(&spi->dev);

		gen_prop_ival(spi->dev.name, "BITS_PER_WORD", spi->bits_per_word);
		gen_prop_ival(spi->dev.name, "CLK_DIV", spi->clk_div);
		for (int j = 0; j < sizeof(spi->spi_devs) / sizeof(spi->spi_devs[0]); j ++) {
			const struct spi_dev_conf *spi_dev = &spi->spi_devs[j];
			if (spi_dev->status == ENABLED) {
				gen_device_conf(&spi_dev->dev);

				gen_prop_ival(spi_dev->dev.name, "BITS_PER_WORD", spi_dev->bits_per_word);
				gen_prop_ival(spi_dev->dev.name, "CLK_FREQ", spi_dev->clk_freq);
				gen_prop_ival(spi_dev->dev.name, "BUS_NUM", spi_dev->bus_num);
				gen_prop_ival(spi_dev->dev.name, "IDX", spi_dev->idx);
			}

		}

		printf("\n");
	}

	/* I2C */
	for (i = 0; i < i2c_conf->array_size; i++) {
		i2c = &((const struct i2c_conf *)i2c_conf->ptr)[i];

		if (i2c->status != ENABLED) {
			continue;
		}

		gen_device_conf(&i2c->dev);

		printf("\n");
	}

	/* PWM */
	for (i = 0; i < pwm_conf->array_size; i++) {
		pwm = &((const struct pwm_conf *)pwm_conf->ptr)[i];

		gen_device_conf(&pwm->dev);

		gen_field_func(pwm->name, "CHANNEL", &pwm->channel);
		gen_field_func(pwm->name, "TIM", &pwm->instance);
		gen_field_int(pwm->name, "SERVO_POS", &pwm->servo_low);
		gen_field_int(pwm->name, "SERVO_POS", &pwm->servo_high);

		printf("\n");
	}

	/* MMC */
	for (i = 0; i < mmc_conf->array_size; i++) {
		mmc = &((const struct mmc_conf *)mmc_conf->ptr)[i];

		if (mmc->status != ENABLED) {
			continue;
		}

		gen_device_conf(&mmc->dev);

		printf("\n");
	}

	/* LED */
	for (i = 0; i < led_conf->array_size; i++) {
		led = &((const struct led_conf *)led_conf->ptr)[i];

		if (led->status != ENABLED) {
			continue;
		}

		gen_dev_enabled(led->name);
		gen_field_gpio_out(led->name, led->port.val, led->pin.val,
		    led->level.val);

		printf("\n");
	}

	/* LED info for leddrv library */
	int led_quantity = 0;

	printf("#define CONF_LED_LIST");
	for (i = 0; i < led_conf->array_size; i++) {
		led = &((const struct led_conf *)led_conf->ptr)[i];

		if (led->status != ENABLED) {
			continue;
		}

		led_quantity += 1;

		printf("%s", (i == 0) ? " " : ", ");
		printf("%s", led->name);
	}
	printf("\n");

	printf("#define CONF_LED_QUANTITY %i\n", led_quantity);
	printf("\n");


	/* LCD */
	for (i = 0; i < lcd_conf->array_size; i++) {
		lcd = &((const struct lcd_conf *)lcd_conf->ptr)[i];

		gen_device_conf(&lcd->dev);

		gen_prop_ival(lcd->dev.name, "RES_X", lcd->width);
		gen_prop_ival(lcd->dev.name, "RES_Y", lcd->height);
		gen_prop_ival(lcd->dev.name, "BITS_PER_PIXEL", lcd->bits_per_pixel);

		printf("\n");
	}
	
	printf("#endif /* BOARD_CONFIG_H_ */\n");

	return 0;
}
