/**
 * @file
 * @brief LVGL input devices interface port (LVGL v9)
 *
 * @date 27.04.2026
 * @author Ruslan Nafikov
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <util/log.h>
#include <drivers/input/input_dev.h>
#include "lvgl.h"
#include <stdio.h>

static bool left_button_down = false;
static int32_t last_x = 0, last_y = 0;
static int inp_fd;

void lvgl_port_input_dev_read(lv_indev_t *indev, lv_indev_data_t *data) {
    (void) indev;
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
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
        last_x += (int16_t)((ev.value >> 16) & 0xffff);
        last_y -= (int16_t)(ev.value & 0xffff);
        last_x = normalize_coord(last_x, 0, lv_display_get_horizontal_resolution(lv_display_get_default()));
        last_y = normalize_coord(last_y, 0, lv_display_get_vertical_resolution(lv_display_get_default()));
        left_button_down = !!(ev.type & MOUSE_BUTTON_LEFT);
    }

}

void lvgl_port_touchscreen_handle(void) {
    struct input_event ev;
    while (1) {
        if (read(inp_fd, &ev, sizeof ev) <= 0) {
            break;
        }
        switch (ev.type & ~TS_EVENT_NEXT) {
        case TS_TOUCH_1:
            last_x = (int32_t) ((ev.value >> 16) & 0xffff);
            last_y = (int32_t) ev.value & 0xffff;
            last_x = normalize_coord(last_x, 0, LV_HOR_RES_MAX);
            last_y = normalize_coord(last_y, 0, LV_VER_RES_MAX);
            left_button_down = true;
            break;
        case TS_TOUCH_1_RELEASED:
            left_button_down = false;
            break;
        default:
            continue;
        }
    }
}