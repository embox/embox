/*
 * @file
 *
 * @date 15.04.2020
 * @author Alexander Kalmuk
 */

// #include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <util/math.h>
#include <kernel/irq.h>
#include <drivers/input/input_dev.h>
#include <drivers/video/fb.h>

#include "stm32f429i_discovery_ts.h"

#define STM32_TS_INT_PIN   STMPE811_INT_PIN
#define STM32_TS_IRQ       OPTION_GET(NUMBER, ts_irq)
static_assert(STM32_TS_IRQ == STMPE811_INT_EXTI, "");

EMBOX_UNIT_INIT(stm32_ts_init);

struct stm32_ts_indev {
	int touch_active[2];
	struct input_dev input_dev;
};

static int stm32_ts_start(struct input_dev *dev) {
	int ret;
	struct fb_info *fb;

	fb = fb_lookup(0);
	if (!fb) {
		log_error("fb_lookup failed");
		return -1;
	}

	ret = BSP_TS_Init(fb->var.xres, fb->var.yres);
	if (ret != TS_OK) {
		log_error("BSP_TS_Init failed");
		return -1;
	}

	BSP_TS_ITConfig();

	return 0;
}

void stm32_ts_poll(struct input_dev *dev) {
	TS_StateTypeDef ts_state;
	struct stm32_ts_indev *ts_dev = (struct stm32_ts_indev *) dev->data;
	struct input_event ev;
	int i;
	/* Only two fingers supported at once on the screen. */
	int touch[2] = {0, 0};

	assert(dev && ts_dev);

	BSP_TS_GetState(&ts_state);

    for (i = 0; i < min(ts_state.TouchDetected, 2); i++) {
        ev.type = (TS_TOUCH_1 + i);

        ev.value = (ts_state.X << 16) | (ts_state.Y & 0xffff);
        input_dev_report_event(dev, &ev);

        touch[i] = 1;

//         printf("type %d . x %d . y %d\n", ev.type, ts_state.X, ts_state.Y);
    }

    for (i = 0; i < 2; i++) {
        if (touch[i]) {
            ts_dev->touch_active[i] = 1;
        } else {
            if (ts_dev->touch_active[i]) {
                ev.type = TS_TOUCH_1_RELEASED + i;
                input_dev_report_event(dev, &ev);
            }
            ts_dev->touch_active[i] = 0;
        }
    }
}

static irq_return_t stm32_ts_irq_hnd(unsigned int irq_nr, void *data) {
 	struct input_dev *dev = (struct input_dev *) data;

 	stm32_ts_poll(dev);

    BSP_TS_ITClear();
    __HAL_GPIO_EXTI_CLEAR_IT(STM32_TS_INT_PIN);

 	return IRQ_HANDLED;
}

static const struct input_dev_ops stm32_ts_input_ops = {
	/* Nothing to do for now, TS started at module init and running forever. */
};

static struct stm32_ts_indev stm32_ts_dev = {
	.input_dev = {
		.ops = &stm32_ts_input_ops,
		.name = "stm32-ts",
		.type = INPUT_DEV_TOUCHSCREEN,
	},
};

static int stm32_ts_init(void) {
	int ret = 0;

    ret = irq_attach(STM32_TS_IRQ, stm32_ts_irq_hnd, 0,
                     &stm32_ts_dev.input_dev, "stm32 touchscreen");
    if (ret != 0) {
        log_error("irq_attach failed");
        return ret;
    }

	ret = input_dev_register(&stm32_ts_dev.input_dev);
	if (ret != 0) {
		log_error("input_dev_register failed");
		goto err_irq_detach;
	}
	stm32_ts_dev.input_dev.data = (void *) &stm32_ts_dev;

	ret = stm32_ts_start(NULL);
	if (ret != 0) {
		log_error("stm32_ts_start failed");
		goto err_irq_detach;
	}

	return 0;

 err_irq_detach:
     irq_detach(STM32_TS_IRQ, &stm32_ts_dev.input_dev);
 	return ret;
}

STATIC_IRQ_ATTACH(STM32_TS_IRQ, stm32_ts_irq_hnd, &stm32_ts_dev.input_dev);
