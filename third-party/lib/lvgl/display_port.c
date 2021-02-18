/**
 * @file
 * @brief LVGL display interface port
 * @defails Based on lv_drivers/display/fbdev.c
 *
 * @date 15.02.2021
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <util/log.h>
#include <drivers/video/fb.h>
#include <linux/fb.h> /* For FBIOGET_FSCREENINFO */

#include "lvgl.h"

static uint8_t *fbp = NULL;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;

void lvgl_port_fbdev_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
	    lv_color_t *color_p) {
	int32_t act_x1, act_y1, act_x2, act_y2;
	long int location = 0;
	lv_coord_t w;

	if (fbp == NULL ||
		    area->x2 < 0 ||
		    area->y2 < 0 ||
		    area->x1 > (int32_t)vinfo.xres - 1 ||
		    area->y1 > (int32_t)vinfo.yres - 1) {
		lv_disp_flush_ready(disp_drv);
		return;
	}

	/* Truncate the area to the screen */
	act_x1 = area->x1 < 0 ? 0 : area->x1;
	act_y1 = area->y1 < 0 ? 0 : area->y1;
	act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
	act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;

	w = (act_x2 - act_x1 + 1);

	if (vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
		uint32_t * fbp32 = (uint32_t *)fbp;
		int32_t y;

		for(y = act_y1; y <= act_y2; y++) {
			location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
			memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
			color_p += w;
		}
	} else if (vinfo.bits_per_pixel == 16) {
		uint16_t * fbp16 = (uint16_t *)fbp;
		int32_t y;

		for(y = act_y1; y <= act_y2; y++) {
			location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 2;
			memcpy(&fbp16[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 2);
			color_p += w;
		}
	} else {
		log_error("Unsupported bits_per_pixel=%d", vinfo.bits_per_pixel);
	}

	lv_disp_flush_ready(disp_drv);
}

int lvgl_port_fbdev_init(const char *fb_path) {
	int fbfd;
	size_t fb_size;

	fbfd = open(fb_path, O_RDWR);
	if (fbfd < 0) {
		log_error("Error open framebuffer %s", fb_path);
		return -1;
	}
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		log_error("Error reading finfo for framebuffer %s", fb_path);
		return -1;
	}
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		log_error("Error reading vinfo for framebuffer %s", fb_path);
		return -1;
	}

	fb_size = finfo.line_length * vinfo.yres;

	fbp = (uint8_t *) mmap(0, fb_size, PROT_READ | PROT_WRITE,
				              MAP_SHARED, fbfd, 0);
	if (fbp == MAP_FAILED) {
		log_error("Error mmap framebuffer %s", fb_path);
	}

	log_debug("framebuffer %s mapped successfully", fb_path);

	return 0;
}
