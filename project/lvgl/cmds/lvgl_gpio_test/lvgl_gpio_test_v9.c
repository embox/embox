/**
 * @file
 * @brief LVGL GPIO test (LVGL v9)
 *
 * @date 14.06.2026
 * @author Ruslan Nafikov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kernel/time/sys_timer.h>
#include <lib/leddrv.h>
#include <lib/libds/array.h>

#include "lvgl.h"
#include "lvgl_port_v9.h"

static char *fb_path = "/dev/fb0";
static char *input_dev_path = "/dev/stm32-ts";

static lv_color_t *buf1_1 = (lv_color_t *)(0xD0000000 + (320 * 240 * 2));

#define PIN_PG13_NAME "LED PG13"
#define PIN_PG14_NAME "LED PG14"

static void input_dev_touchscreen_handler(lv_timer_t *t) {
	lvgl_port_touchscreen_handle();
}

static int hal_init(void) {
	lv_display_t *disp;
	lv_indev_t *indev;

	if (lvgl_port_fbdev_init(fb_path) < 0) {
		fprintf(stderr, "Failed to init framebuffer %s\n", fb_path);
		return -1;
	}

	disp = lv_display_create(lvgl_port_fbdev_width(), lvgl_port_fbdev_height());

	int32_t hor = lv_display_get_horizontal_resolution(disp);
	int32_t ver = lv_display_get_vertical_resolution(disp);

	lv_display_set_flush_cb(disp, lvgl_port_fbdev_flush);
	lv_display_set_buffers(disp, buf1_1, NULL, hor * ver * sizeof(lv_color_t),
	    LV_DISPLAY_RENDER_MODE_FULL);

	if (lvgl_port_input_dev_init(input_dev_path) < 0) {
		fprintf(stderr, "Error open input device %s\n", input_dev_path);
		return -1;
	}

	indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, lvgl_port_input_dev_read);
	lv_indev_set_display(indev, disp);

	lv_timer_create(input_dev_touchscreen_handler, 10, NULL);

	return 0;
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

void lv_checkbox_demo(void) {
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

	lv_init();

	if (0 != hal_init()) {
		fprintf(stderr, "lvgl hal_init failed\n");
		return -1;
	}

	lv_checkbox_demo();

	sys_timer_set(&timer, SYS_TIMER_PERIODIC, 50, lvgl_timer_handler, NULL);

	while (1) {
		lv_timer_handler();
		usleep(5 * 1000);
	}

	return 0;
}
