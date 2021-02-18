/**
 * @file
 * @brief LVGL input devices interface port
 *
 * @date 15.02.2021
 * @author Alexander Kalmuk
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <util/log.h>
#include <drivers/input/input_dev.h>

#include "lvgl.h"

static bool left_button_down = false;
static int16_t last_x = 0, last_y = 0;
static int inp_fd;

bool lvgl_port_input_dev_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
	(void) indev_drv;      /*Unused*/

	/*Store the collected data*/
	data->point.x = last_x;
	data->point.y = last_y;
	data->state = left_button_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

	return false;
}

int lvgl_port_input_dev_init(const char *path) {
	inp_fd = open(path, O_RDONLY | O_NONBLOCK);
	if (inp_fd == -1) {
		log_error("Cannot find mouse \"%s\"", path);
		return -1;
	}

	return 0;
}

static int normalize_coord(int x, int a, int b) {
	if (x < a) {
		return a;
	} else if (x > b - 1) {
		return b - 1;
	} else {
		return x;
	}
}

void lvgl_port_mouse_handle(void) {
	struct input_event ev;

	while (1) {
		if (read(inp_fd, &ev, sizeof ev) <= 0) {
			break;
		}

		last_x += (ev.value >> 16) & 0xffff;
		last_y -= ev.value & 0xffff;

		last_x = normalize_coord(last_x, 0, LV_HOR_RES_MAX);
		last_y = normalize_coord(last_y, 0, LV_VER_RES_MAX);

		left_button_down = !!(ev.type & MOUSE_BUTTON_LEFT);
	}
}
