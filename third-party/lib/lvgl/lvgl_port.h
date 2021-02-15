/**
 * @file
 * @brief LVGL porting layer
 * @details LVGL assumes direct calls of init functions such as
 *          display or input device initiazation from the application.
 *          So each application should include this file like:
 *             #include "lvgl_port.h"
 *
 * @date 15.02.2021
 * @author Alexander Kalmuk
 */

#ifndef THIRD_PARTY_LIB_LVGL_LVGL_PORT_H_
#define THIRD_PARTY_LIB_LVGL_LVGL_PORT_H_

#include "lvgl.h"

extern int lvgl_port_fbdev_init(const char *fb_path);
extern void lvgl_port_fbdev_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
	    lv_color_t *color_p);

#endif /* THIRD_PARTY_LIB_LVGL_LVGL_PORT_H_ */
