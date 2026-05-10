/**
 * @file
 * @brief LVGL display interface port for STM32 (LVGL v9)
 * @details Based on lv_drivers/display/fbdev.c
 *
 * @date 27.04.2026
 * @author Ruslan Nafikov
 */
#include <unistd.h>
#include <util/log.h>
#include <drivers/video/fb.h>
#if defined STM32F746xx || defined STM32F769xx
#include <stm32f7xx_hal.h>
#elif defined STM32H745xx
#include <stm32h7xx_hal.h>
#elif defined (STM32F429xx)
#include <stm32f4xx_hal.h>
#endif
#include "lvgl.h"

extern uint8_t *lv_fbp;
extern struct fb_var_screeninfo lv_vinfo;
extern struct fb_fix_screeninfo lv_finfo;

void lvgl_port_fbdev_flush(lv_display_t *disp, const lv_area_t *area,
        uint8_t *px_map) {
    int32_t act_x1, act_y1, act_x2, act_y2;
    long int location = 0;
    int32_t w;

    if (lv_fbp == NULL ||
            area->x2 < 0 ||
            area->y2 < 0 ||
            area->x1 > (int32_t)lv_vinfo.xres - 1 ||
            area->y1 > (int32_t)lv_vinfo.yres - 1) {
        lv_display_flush_ready(disp);
        return;
    }

    act_x1 = area->x1 < 0 ? 0 : area->x1;
    act_y1 = area->y1 < 0 ? 0 : area->y1;
    act_x2 = area->x2 > (int32_t)lv_vinfo.xres - 1 ? (int32_t)lv_vinfo.xres - 1 : area->x2;
    act_y2 = area->y2 > (int32_t)lv_vinfo.yres - 1 ? (int32_t)lv_vinfo.yres - 1 : area->y2;
    w = (act_x2 - act_x1 + 1);

    if (lv_vinfo.bits_per_pixel == 32 || lv_vinfo.bits_per_pixel == 24) {
        uint32_t *lv_fbp32 = (uint32_t *)lv_fbp;
        int32_t y;
        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + lv_vinfo.xoffset) +
                       (y + lv_vinfo.yoffset) * lv_finfo.line_length / 4;
            memcpy(&lv_fbp32[location], px_map, w * 4);
            px_map += w * 4;
        }
    } else if (lv_vinfo.bits_per_pixel == 16) {
        uint16_t *lv_fbp16 = (uint16_t *)lv_fbp;
        int32_t y;
        for (y = act_y1; y <= act_y2; y++) {
            location = (act_x1 + lv_vinfo.xoffset) +
                       (y + lv_vinfo.yoffset) * lv_finfo.line_length / 2;
            memcpy(&lv_fbp16[location], px_map, w * 2);
            px_map += w * 2;
        }
    } else {
        log_error("Unsupported bits_per_pixel=%d", lv_vinfo.bits_per_pixel);
    }

#if defined STM32F746xx || defined STM32F769xx || defined STM32H745xx
    SCB_CleanDCache();
#endif

    lv_display_flush_ready(disp);
}