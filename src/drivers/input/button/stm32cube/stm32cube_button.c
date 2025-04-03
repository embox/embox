/*
 * @file
 *
 * @date 15.04.2020
 * @author Alexander Kalmuk
 */

#include <util/log.h>

#include <errno.h>
#include <assert.h>

#include <kernel/irq.h>
#include <drivers/input/input_dev.h>

#include <drivers/gpio.h>

#include <embox/unit.h>
#include <framework/mod/options.h>


EMBOX_UNIT_INIT(stm32cube_bt_init);

#define USER_BUTTON_PORT       OPTION_GET(NUMBER,pin_port)
#define USER_BUTTON_PIN_MASK   (1 << OPTION_GET(NUMBER,pin_num))

struct stm32cube_bt_indev {
	struct input_dev input_dev;
};

static struct stm32cube_bt_indev stm32cube_bt_dev;

static void stm32cube_bt_irq_hnd(void *data) {
	struct input_dev *dev;
	gpio_mask_t status;
	struct input_event ev;

	dev = &((struct stm32cube_bt_indev *)data)->input_dev;

	status = gpio_get(USER_BUTTON_PORT, USER_BUTTON_PIN_MASK);
	if (status) {
		ev.type = USER_BUTTON_PRESSED;
		log_debug("pressed");
	} else {
		ev.type = USER_BUTTON_UNPRESSED;
		log_debug("unpressed");
	}
	ev.value = 0;

	input_dev_report_event(dev, &ev);
}

static int stm32cube_bt_start(struct input_dev *dev) {
	gpio_setup_mode(USER_BUTTON_PORT, USER_BUTTON_PIN_MASK, GPIO_MODE_INT_MODE_RISING_FALLING);
	if (0 > gpio_irq_attach(USER_BUTTON_PORT, USER_BUTTON_PIN_MASK, stm32cube_bt_irq_hnd, &stm32cube_bt_dev)) {
		log_error("Failed to attach IRQ handler");
		return -1;
	}

	return 0;
}



static const struct input_dev_ops stm32cube_bt_input_ops = {
	/* Nothing to do for now, TS started at module init and running forever. */
};

static struct stm32cube_bt_indev stm32cube_bt_dev = {
	.input_dev = {
		.ops = &stm32cube_bt_input_ops,
		.name = "user-bt",
		.type = INPUT_DEV_BUTTON,
	},
};

static int stm32cube_bt_init(void) {
	int ret = 0;

	ret = input_dev_register(&stm32cube_bt_dev.input_dev);
	if (ret != 0) {
		log_error("input_dev_register failed");
		goto err_out;
	}
	stm32cube_bt_dev.input_dev.data = (void *) &stm32cube_bt_dev;

	ret = stm32cube_bt_start(NULL);
	if (ret != 0) {
		log_error("stm32_ts_start failed");

	}

err_out:
	return ret;
}

