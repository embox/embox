/**
 * @file
 * @brief LVGL demo
 *
 * @date 15.02.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kernel/time/timer.h>

#include "lvgl.h"
#if LVGL_VERSION_MAJOR == 7
#include "lv_examples.h"
#else
#include "lv_demo.h"
#endif

#include "lvgl_port.h"

static char *fb_path;
static char *input_dev_path;
static int input_is_mouse = 0;

static lv_color_t *buf1_1;
#if LVGL_VERSION_MAJOR == 7
static lv_disp_buf_t disp_buf1;
#elif LVGL_VERSION_MAJOR == 8
static lv_disp_draw_buf_t disp_buf1;
#else
#error "wrong LVGL version"
#endif

#if LVGL_VERSION_MAJOR == 7
static void input_dev_mouse_handler(lv_task_t *t) {
	lvgl_port_mouse_handle();
}

static void input_dev_touchscreen_handler(lv_task_t *t) {
	lvgl_port_touchscreen_handle();
}
#else
static void input_dev_mouse_handler(lv_timer_t *t) {
	lvgl_port_mouse_handle();
}

static void input_dev_touchscreen_handler(lv_timer_t *t) {
	lvgl_port_touchscreen_handle();
}
#endif

static int hal_init(void) {
	static lv_disp_drv_t disp_drv;
	static lv_indev_drv_t indev_drv;
	lv_indev_t *mouse_indev;
	lv_obj_t *cursor_obj;

	if (lvgl_port_fbdev_init(fb_path) < 0) {
		fprintf(stderr, "Failed to init framebuffer %s\n", fb_path);
		return -1;
	}

	buf1_1 = malloc(LV_HOR_RES_MAX * LV_VER_RES_MAX * 4);
	if (!buf1_1) {
		fprintf(stderr, "Error allocation buffer for LVGL display\n");
		return -1;
	}
#if LVGL_VERSION_MAJOR == 7
	lv_disp_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
#else
	lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
#endif
	/*Create a display*/
	lv_disp_drv_init(&disp_drv);
#if LVGL_VERSION_MAJOR == 7
	disp_drv.buffer = &disp_buf1;
#else
    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
	disp_drv.draw_buf = &disp_buf1;
#endif
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
	mouse_indev = lv_indev_drv_register(&indev_drv);

	if (input_is_mouse) {
		/*Set a cursor for the mouse*/
		LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
#if LVGL_VERSION_MAJOR == 7
		cursor_obj = lv_img_create(lv_scr_act(), NULL);
#else
		cursor_obj = lv_img_create(lv_scr_act());
#endif
		lv_img_set_src(cursor_obj, &mouse_cursor_icon);
		lv_indev_set_cursor(mouse_indev, cursor_obj);
	}
#if LVGL_VERSION_MAJOR == 7
	if (input_is_mouse) {
		lv_task_create(input_dev_mouse_handler, 10, LV_TASK_PRIO_HIGH, NULL);

	} else {
		lv_task_create(input_dev_touchscreen_handler, 10, LV_TASK_PRIO_HIGH, NULL);
	}
#else
	if (input_is_mouse) {
		lv_timer_create(input_dev_mouse_handler, 10, NULL);

	} else {
		lv_timer_create(input_dev_touchscreen_handler, 10, NULL);
	}
#endif
	return 0;

err_free:
	free(buf1_1);
	return -1;
}

static inline void lvgl_timer_handler(struct sys_timer *timer, void *param) {
	(void) timer;
	(void) param;

	lv_tick_inc(50);
}

static void print_usage(void) {
	printf("Usage: lvgl_demo [-t] fb input_dev\n"
	       "\t -t: Ifspecified, this means input device is touchscreen.\n"
	       "\t     Othervice, it's mouse.\n"
	);
}

int main(int argc, char **argv) {
	int opt;
	struct sys_timer *timer;

	input_is_mouse = 1;

	if (argc < 3) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "ht"))) {
		switch (opt) {
		case 't':
			input_is_mouse = 0;
			break;
		case 'h':
			print_usage();
			/* fallthrough */
		default:
			return 0;
		}
	}

	fb_path = argv[argc - 2];
	input_dev_path = argv[argc - 1];

	/*Initialize LVGL*/
	lv_init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	if (0 != hal_init()) {
		fprintf(stderr, "lvgl hal_init failed\n");
		return -1;
	}

	lv_demo_widgets();

	sys_timer_set(&timer, SYS_TIMER_PERIODIC, 50, lvgl_timer_handler, NULL);

	while (1) {
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/
#if LVGL_VERSION_MAJOR == 7
		lv_task_handler();
#else
		lv_timer_handler();
#endif
		usleep(5 * 1000);
	}

	return 0;
}
