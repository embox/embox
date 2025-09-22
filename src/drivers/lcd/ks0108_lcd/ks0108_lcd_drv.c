/*
 * @file
 *
 * @date Sep 19, 2025
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <drivers/video/fb.h>

#include <config/board_config.h>

#include <framework/mod/options.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(ks0108_lcd_drv_init);

extern void ks0108_lcd_init(void);

uint8_t ks0108_lcd_buf[(CONF_LCD_RES_X * CONF_LCD_RES_Y) / 8];

static int ks0108_lcd_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {

	return 0;
}

static int ks0108_lcd_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	var->xres = 128;
	var->yres = 64;
	var->bits_per_pixel = 1;

	return 0;
}

static const struct fb_ops ks0108_lcd_ops = {
	.fb_get_var = ks0108_lcd_get_var,
	.fb_set_var = ks0108_lcd_set_var,
};

static int ks0108_lcd_drv_init(void) {
	struct fb_info *info;

	info = fb_create(&ks0108_lcd_ops, (char *)ks0108_lcd_buf, sizeof(ks0108_lcd_buf));
	if (info == NULL) {

		return -ENOMEM;
	}

	ks0108_lcd_init();

	return 0;
}
