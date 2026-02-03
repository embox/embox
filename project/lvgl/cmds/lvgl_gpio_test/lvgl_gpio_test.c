/**
 * @file
 * @brief LVGL test
 *
 * @date 14.12.2021
 * @author Dmitriy Kocherygin
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <kernel/time/sys_timer.h>
#include <lib/leddrv.h>
#include <lib/libds/array.h>

#include "lvgl.h"
#include "lvgl_port.h"

static char *fb_path = "/dev/fb0";
static char *input_dev_path = "/dev/stm32-ts";

static lv_color_t *buf1_1 = (lv_color_t *)(0xD0000000 + (320 * 240 * 2));

#define PIN_PG13_NAME "LED PG13"
#define PIN_PG14_NAME "LED PG14"

static void input_dev_touchscreen_handler(lv_timer_t *t) {
	lvgl_port_touchscreen_handle();
}

static int hal_init(void) {
	static lv_disp_drv_t disp_drv;
	static lv_indev_drv_t indev_drv;
	static lv_disp_draw_buf_t disp_buf1;

	if (lvgl_port_fbdev_init(fb_path) < 0) {
		fprintf(stderr, "Failed to init framebuffer %s\n", fb_path);
		return -1;
	}

	//	buf1_1 =  malloc(LV_HOR_RES_MAX * LV_VER_RES_MAX * 2);
	if (!buf1_1) {
		fprintf(stderr, "Error allocation buffer for LVGL display\n");
		return -1;
	}

	lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL,
	    LV_HOR_RES_MAX * LV_VER_RES_MAX);

	/*Create a display*/
	lv_disp_drv_init(&disp_drv);

	/*Set the resolution of the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;
	disp_drv.ver_res = LV_VER_RES_MAX;
	disp_drv.draw_buf = &disp_buf1;

	disp_drv.flush_cb = lvgl_port_fbdev_flush;
	lv_disp_drv_register(&disp_drv);

	if (lvgl_port_input_dev_init(input_dev_path) < 0) {
		fprintf(stderr, "Error open input device %s\n", input_dev_path);
		goto err_free;
	}

	/* Add the mouse as input device */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = lvgl_port_input_dev_read;
	lv_indev_drv_register(&indev_drv);

	lv_timer_create(input_dev_touchscreen_handler, 10, NULL);

	return 0;

err_free:
	free(buf1_1);
	return -1;
}

static inline void lvgl_timer_handler(struct sys_timer *timer, void *param) {
	(void)timer;
	(void)param;

	lv_tick_inc(50);
}

static void event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);

	if (code == LV_EVENT_VALUE_CHANGED) {
		const char *txt = lv_checkbox_get_text(obj);
		uint8_t state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? 1 : 0;

		int8_t pin = -1;

		if (strcmp(txt, PIN_PG13_NAME) == 0) {
			pin = 0;
		}
		if (strcmp(txt, PIN_PG14_NAME) == 0) {
			pin = 1;
		}

		if (state) {
			leddrv_led_on(pin);
		}
		else {
			leddrv_led_off(pin);
		}
	}
}

void lv_checkbox(void) {
	lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_CENTER,
	    LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

	lv_obj_t *cb;
	cb = lv_checkbox_create(lv_scr_act());
	lv_checkbox_set_text(cb, PIN_PG13_NAME);
	lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

	cb = lv_checkbox_create(lv_scr_act());
	lv_checkbox_set_text(cb, PIN_PG14_NAME);
	lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

	lv_obj_update_layout(cb);
}

int main(int argc, char **argv) {
	struct sys_timer *timer;

	/*Initialize LVGL*/
	lv_init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	if (0 != hal_init()) {
		fprintf(stderr, "lvgl hal_init failed\n");
		return -1;
	}

	lv_checkbox();

	sys_timer_set(&timer, SYS_TIMER_PERIODIC, 50, lvgl_timer_handler, NULL);

	while (1) {
		/* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
		lv_timer_handler();
		usleep(5 * 1000);
	}

	return 0;
}
