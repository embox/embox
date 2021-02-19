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

uint8_t *lv_fbp = NULL;
struct fb_var_screeninfo lv_vinfo;
struct fb_fix_screeninfo lv_finfo;

int lvgl_port_fbdev_init(const char *fb_path) {
	int fbfd;
	size_t fb_size;

	fbfd = open(fb_path, O_RDWR);
	if (fbfd < 0) {
		log_error("Error open framebuffer %s", fb_path);
		return -1;
	}
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &lv_finfo) == -1) {
		log_error("Error reading finfo for framebuffer %s", fb_path);
		return -1;
	}
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &lv_vinfo) == -1) {
		log_error("Error reading vinfo for framebuffer %s", fb_path);
		return -1;
	}

	fb_size = lv_finfo.line_length * lv_vinfo.yres;

	lv_fbp = (uint8_t *) mmap(0, fb_size, PROT_READ | PROT_WRITE,
				              MAP_SHARED, fbfd, 0);
	if (lv_fbp == MAP_FAILED) {
		log_error("Error mmap framebuffer %s", fb_path);
	}

	log_debug("framebuffer %s mapped successfully", fb_path);

	return 0;
}
