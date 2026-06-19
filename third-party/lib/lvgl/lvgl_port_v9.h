/**
 * @file
 * @brief LVGL porting layer (LVGL v9)
 *
 * @date 27.04.2026
 * @author Ruslan Nafikov
 */
#ifndef THIRD_PARTY_LIB_LVGL_LVGL_PORT_V9_H_
#define THIRD_PARTY_LIB_LVGL_LVGL_PORT_V9_H_

#include "lvgl.h"

/* Display */
extern int lvgl_port_fbdev_init(const char *fb_path);
extern int lvgl_port_fbdev_width(void);
extern int lvgl_port_fbdev_height(void);
extern void lvgl_port_fbdev_flush(lv_display_t *disp, const lv_area_t *area,
        uint8_t *px_map);

/* Input device */
extern int lvgl_port_input_dev_init(const char *path);
extern void lvgl_port_input_dev_read(lv_indev_t *indev, lv_indev_data_t *data);
extern void lvgl_port_mouse_handle(void);
extern void lvgl_port_touchscreen_handle(void);

#endif /* THIRD_PARTY_LIB_LVGL_LVGL_PORT_V9_H_ */