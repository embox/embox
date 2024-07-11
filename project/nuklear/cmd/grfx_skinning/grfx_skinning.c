/* This file provides an widget
 example of work nuklear on OS Embox */

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

#define NK_PRIVATE
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "nk_embox_render.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
/* macros */
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

struct device {
	struct nk_buffer cmds;
	struct nk_draw_null_texture null;
};

struct nk_canvas {
	struct nk_command_buffer *painter;
	struct nk_vec2 item_spacing;
	struct nk_vec2 panel_padding;
	struct nk_style_item window_background;
};

struct media {
	uint32_t skin;
	struct nk_image menu;
	struct nk_image check;
	struct nk_image check_cursor;
	struct nk_image option;
	struct nk_image option_cursor;
	struct nk_image header;
	struct nk_image window;
	struct nk_image scrollbar_inc_button;
	struct nk_image scrollbar_inc_button_hover;
	struct nk_image scrollbar_dec_button;
	struct nk_image scrollbar_dec_button_hover;
	struct nk_image button;
	struct nk_image button_hover;
	struct nk_image button_active;
	struct nk_image tab_minimize;
	struct nk_image tab_maximize;
	struct nk_image slider;
	struct nk_image slider_hover;
	struct nk_image slider_active;
};

static float your_text_width_calculation(nk_handle handle, float height,
		const char *text, int len) {
	return 8;
}

static uint32_t image_load(char *file_name) {
	/* load image for the widget items */
	int w, h, f;
	unsigned char *data;

	data = stbi_load(file_name, &w, &h, &f, 0);
	if (data == NULL) {
		printf("\nstbi_load doesn't work. :(\n");
		return 0;
	} else {
		printf(
				"\nLoaded image: id = %p   width = %i\theight = %i\tformat = %i\n",
				data, w, h, f);
	}
	return (uint32_t) data;
}

int main(int argc, char *argv[]) {
	long int screensize = 0;
	uint8_t *fbp = 0;
	struct fb_info *fb_info;
	struct nk_color rgb_white = { .a = 0xff, .r = 0xff, .g = 0xff, .b = 0xff};

	fb_info = fb_lookup(0);

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n", fb_info->var.xres, fb_info->var.yres,
			fb_info->var.bits_per_pixel);

	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres
			* fb_info->var.bits_per_pixel / 8;

	/* Map the device to memory */
	fbp = (uint8_t *)mmap_device_memory((void *)fb_info->screen_base,
	    screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
	    (uintptr_t)fb_info->screen_base);
	if ((int) fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	struct fb_fillrect rect;
	rect.dx = 0;
	rect.dy = 0;
	rect.width = fb_info->var.xres;
	rect.height = fb_info->var.yres;
	rect.rop = ROP_COPY;
	rect.color = rgba_to_device_color(fb_info, &rgb_white);
	fb_fillrect(fb_info, &rect);

	/* GUI */
	static struct device device;
	static struct nk_context ctx;
	static struct media media;

	uint32_t width = 0, height = 0;

	static struct nk_user_font font;
	font.userdata.ptr = (void*) font_vga_8x8.data;
	font.height = font_vga_8x8.height;
	font.width = your_text_width_calculation;
	nk_init_default(&ctx, &font);

	width = fb_info->var.yres;
	height = fb_info->var.xres;

	{ /* skin */

		media.skin = image_load("gwen.png");
		media.check = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 32, 15, 15));
		media.check_cursor = nk_subimage_id(media.skin, 512, 512,
				nk_rect(450, 34, 11, 11));
		media.option = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 64, 15, 15));
		media.option_cursor = nk_subimage_id(media.skin, 512, 512,
				nk_rect(451, 67, 9, 9));
		media.header = nk_subimage_id(media.skin, 512, 512,
				nk_rect(128, 0, 127, 24));
		media.window = nk_subimage_id(media.skin, 512, 512,
				nk_rect(128, 23, 127, 104));
		media.scrollbar_inc_button = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 256, 15, 15));
		media.scrollbar_inc_button_hover = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 320, 15, 15));
		media.scrollbar_dec_button = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 224, 15, 15));
		media.scrollbar_dec_button_hover = nk_subimage_id(media.skin, 512, 512,
				nk_rect(464, 288, 15, 15));
		media.button = nk_subimage_id(media.skin, 512, 512,
				nk_rect(384, 336, 127, 31));
		media.button_hover = nk_subimage_id(media.skin, 512, 512,
				nk_rect(384, 368, 127, 31));
		media.button_active = nk_subimage_id(media.skin, 512, 512,
				nk_rect(384, 400, 127, 31));
		media.tab_minimize = nk_subimage_id(media.skin, 512, 512,
				nk_rect(451, 99, 9, 9));
		media.tab_maximize = nk_subimage_id(media.skin, 512, 512,
				nk_rect(467, 99, 9, 9));
		media.slider = nk_subimage_id(media.skin, 512, 512,
				nk_rect(418, 33, 11, 14));
		media.slider_hover = nk_subimage_id(media.skin, 512, 512,
				nk_rect(418, 49, 11, 14));
		media.slider_active = nk_subimage_id(media.skin, 512, 512,
				nk_rect(418, 64, 11, 14));

		/* window */
		ctx.style.window.background = nk_rgb(204, 204, 204);
		ctx.style.window.fixed_background = nk_style_item_image(media.window);
		ctx.style.window.border_color = nk_rgb(67, 67, 67);
		ctx.style.window.combo_border_color = nk_rgb(67, 67, 67);
		ctx.style.window.contextual_border_color = nk_rgb(67, 67, 67);
		ctx.style.window.menu_border_color = nk_rgb(67, 67, 67);
		ctx.style.window.group_border_color = nk_rgb(67, 67, 67);
		ctx.style.window.tooltip_border_color = nk_rgb(67, 67, 67);
		ctx.style.window.scrollbar_size = nk_vec2(16, 16);
		ctx.style.window.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.window.padding = nk_vec2(8, 4);
		ctx.style.window.border = 3;

		/* window header */
		ctx.style.window.header.normal = nk_style_item_image(media.header);
		ctx.style.window.header.hover = nk_style_item_image(media.header);
		ctx.style.window.header.active = nk_style_item_image(media.header);
		ctx.style.window.header.label_normal = nk_rgb(95, 95, 95);
		ctx.style.window.header.label_hover = nk_rgb(95, 95, 95);
		ctx.style.window.header.label_active = nk_rgb(95, 95, 95);

		/* scrollbar */
		ctx.style.scrollv.normal = nk_style_item_color(nk_rgb(184, 184, 184));
		ctx.style.scrollv.hover = nk_style_item_color(nk_rgb(184, 184, 184));
		ctx.style.scrollv.active = nk_style_item_color(nk_rgb(184, 184, 184));
		ctx.style.scrollv.cursor_normal = nk_style_item_color(
				nk_rgb(220, 220, 220));
		ctx.style.scrollv.cursor_hover = nk_style_item_color(
				nk_rgb(235, 235, 235));
		ctx.style.scrollv.cursor_active = nk_style_item_color(
				nk_rgb(99, 202, 255));
		ctx.style.scrollv.dec_symbol = NK_SYMBOL_NONE;
		ctx.style.scrollv.inc_symbol = NK_SYMBOL_NONE;
		ctx.style.scrollv.show_buttons = nk_true;
		ctx.style.scrollv.border_color = nk_rgb(81, 81, 81);
		ctx.style.scrollv.cursor_border_color = nk_rgb(81, 81, 81);
		ctx.style.scrollv.border = 1;
		ctx.style.scrollv.rounding = 0;
		ctx.style.scrollv.border_cursor = 1;
		ctx.style.scrollv.rounding_cursor = 2;

		/* scrollbar buttons */
		ctx.style.scrollv.inc_button.normal = nk_style_item_image(
				media.scrollbar_inc_button);
		ctx.style.scrollv.inc_button.hover = nk_style_item_image(
				media.scrollbar_inc_button_hover);
		ctx.style.scrollv.inc_button.active = nk_style_item_image(
				media.scrollbar_inc_button_hover);
		ctx.style.scrollv.inc_button.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.inc_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.inc_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.inc_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.inc_button.text_active = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.inc_button.border = 0.0f;

		ctx.style.scrollv.dec_button.normal = nk_style_item_image(
				media.scrollbar_dec_button);
		ctx.style.scrollv.dec_button.hover = nk_style_item_image(
				media.scrollbar_dec_button_hover);
		ctx.style.scrollv.dec_button.active = nk_style_item_image(
				media.scrollbar_dec_button_hover);
		ctx.style.scrollv.dec_button.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.dec_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.dec_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.dec_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.dec_button.text_active = nk_rgba(0, 0, 0, 0);
		ctx.style.scrollv.dec_button.border = 0.0f;

		/* checkbox toggle */
		{
			static struct nk_style_toggle *toggle;
			toggle = &ctx.style.checkbox;
			toggle->normal = nk_style_item_image(media.check);
			toggle->hover = nk_style_item_image(media.check);
			toggle->active = nk_style_item_image(media.check);
			toggle->cursor_normal = nk_style_item_image(media.check_cursor);
			toggle->cursor_hover = nk_style_item_image(media.check_cursor);
			toggle->text_normal = nk_rgb(95, 95, 95);
			toggle->text_hover = nk_rgb(95, 95, 95);
			toggle->text_active = nk_rgb(95, 95, 95);
		}

		/* option toggle */
		{
			static struct nk_style_toggle *toggle;
			toggle = &ctx.style.option;
			toggle->normal = nk_style_item_image(media.option);
			toggle->hover = nk_style_item_image(media.option);
			toggle->active = nk_style_item_image(media.option);
			toggle->cursor_normal = nk_style_item_image(media.option_cursor);
			toggle->cursor_hover = nk_style_item_image(media.option_cursor);
			toggle->text_normal = nk_rgb(95, 95, 95);
			toggle->text_hover = nk_rgb(95, 95, 95);
			toggle->text_active = nk_rgb(95, 95, 95);
		}

		/* default button */
		ctx.style.button.normal = nk_style_item_image(media.button);
		ctx.style.button.hover = nk_style_item_image(media.button_hover);
		ctx.style.button.active = nk_style_item_image(media.button_active);
		ctx.style.button.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.button.text_normal = nk_rgb(95, 95, 95);
		ctx.style.button.text_hover = nk_rgb(95, 95, 95);
		ctx.style.button.text_active = nk_rgb(95, 95, 95);

		/* default text */
		ctx.style.text.color = nk_rgb(95, 95, 95);

		/* contextual button */
		ctx.style.contextual_button.normal = nk_style_item_color(
				nk_rgb(206, 206, 206));
		ctx.style.contextual_button.hover = nk_style_item_color(
				nk_rgb(229, 229, 229));
		ctx.style.contextual_button.active = nk_style_item_color(
				nk_rgb(99, 202, 255));
		ctx.style.contextual_button.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.contextual_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.contextual_button.text_normal = nk_rgb(95, 95, 95);
		ctx.style.contextual_button.text_hover = nk_rgb(95, 95, 95);
		ctx.style.contextual_button.text_active = nk_rgb(95, 95, 95);

		/* menu button */
		ctx.style.menu_button.normal = nk_style_item_color(
				nk_rgb(206, 206, 206));
		ctx.style.menu_button.hover = nk_style_item_color(
				nk_rgb(229, 229, 229));
		ctx.style.menu_button.active = nk_style_item_color(
				nk_rgb(99, 202, 255));
		ctx.style.menu_button.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.menu_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.menu_button.text_normal = nk_rgb(95, 95, 95);
		ctx.style.menu_button.text_hover = nk_rgb(95, 95, 95);
		ctx.style.menu_button.text_active = nk_rgb(95, 95, 95);

		/* tree */
		ctx.style.tab.text = nk_rgb(95, 95, 95);
		ctx.style.tab.tab_minimize_button.normal = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.tab_minimize_button.hover = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.tab_minimize_button.active = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.tab_minimize_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_minimize_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_minimize_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_minimize_button.text_active = nk_rgba(0, 0, 0, 0);

		ctx.style.tab.tab_maximize_button.normal = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.tab_maximize_button.hover = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.tab_maximize_button.active = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.tab_maximize_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_maximize_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_maximize_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.tab_maximize_button.text_active = nk_rgba(0, 0, 0, 0);

		ctx.style.tab.node_minimize_button.normal = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.node_minimize_button.hover = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.node_minimize_button.active = nk_style_item_image(
				media.tab_minimize);
		ctx.style.tab.node_minimize_button.text_background = nk_rgba(0, 0, 0,
				0);
		ctx.style.tab.node_minimize_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.node_minimize_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.node_minimize_button.text_active = nk_rgba(0, 0, 0, 0);

		ctx.style.tab.node_maximize_button.normal = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.node_maximize_button.hover = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.node_maximize_button.active = nk_style_item_image(
				media.tab_maximize);
		ctx.style.tab.node_maximize_button.text_background = nk_rgba(0, 0, 0,
				0);
		ctx.style.tab.node_maximize_button.text_normal = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.node_maximize_button.text_hover = nk_rgba(0, 0, 0, 0);
		ctx.style.tab.node_maximize_button.text_active = nk_rgba(0, 0, 0, 0);

		/* selectable */
		ctx.style.selectable.normal = nk_style_item_color(
				nk_rgb(206, 206, 206));
		ctx.style.selectable.hover = nk_style_item_color(nk_rgb(206, 206, 206));
		ctx.style.selectable.pressed = nk_style_item_color(
				nk_rgb(206, 206, 206));
		ctx.style.selectable.normal_active = nk_style_item_color(
				nk_rgb(185, 205, 248));
		ctx.style.selectable.hover_active = nk_style_item_color(
				nk_rgb(185, 205, 248));
		ctx.style.selectable.pressed_active = nk_style_item_color(
				nk_rgb(185, 205, 248));
		ctx.style.selectable.text_normal = nk_rgb(95, 95, 95);
		ctx.style.selectable.text_hover = nk_rgb(95, 95, 95);
		ctx.style.selectable.text_pressed = nk_rgb(95, 95, 95);
		ctx.style.selectable.text_normal_active = nk_rgb(95, 95, 95);
		ctx.style.selectable.text_hover_active = nk_rgb(95, 95, 95);
		ctx.style.selectable.text_pressed_active = nk_rgb(95, 95, 95);

		/* slider */
		ctx.style.slider.normal = nk_style_item_hide();
		ctx.style.slider.hover = nk_style_item_hide();
		ctx.style.slider.active = nk_style_item_hide();
		ctx.style.slider.bar_normal = nk_rgb(156, 156, 156);
		ctx.style.slider.bar_hover = nk_rgb(156, 156, 156);
		ctx.style.slider.bar_active = nk_rgb(156, 156, 156);
		ctx.style.slider.bar_filled = nk_rgb(156, 156, 156);
		ctx.style.slider.cursor_normal = nk_style_item_image(media.slider);
		ctx.style.slider.cursor_hover = nk_style_item_image(media.slider_hover);
		ctx.style.slider.cursor_active = nk_style_item_image(
				media.slider_active);
		ctx.style.slider.cursor_size = nk_vec2(16.5f, 21);
		ctx.style.slider.bar_height = 1;

		/* progressbar */
		ctx.style.progress.normal = nk_style_item_color(nk_rgb(231, 231, 231));
		ctx.style.progress.hover = nk_style_item_color(nk_rgb(231, 231, 231));
		ctx.style.progress.active = nk_style_item_color(nk_rgb(231, 231, 231));
		ctx.style.progress.cursor_normal = nk_style_item_color(
				nk_rgb(63, 242, 93));
		ctx.style.progress.cursor_hover = nk_style_item_color(
				nk_rgb(63, 242, 93));
		ctx.style.progress.cursor_active = nk_style_item_color(
				nk_rgb(63, 242, 93));
		ctx.style.progress.border_color = nk_rgb(114, 116, 115);
		ctx.style.progress.padding = nk_vec2(0, 0);
		ctx.style.progress.border = 2;
		ctx.style.progress.rounding = 1;

		/* combo */
		ctx.style.combo.normal = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.combo.hover = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.combo.active = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.combo.border_color = nk_rgb(95, 95, 95);
		ctx.style.combo.label_normal = nk_rgb(95, 95, 95);
		ctx.style.combo.label_hover = nk_rgb(95, 95, 95);
		ctx.style.combo.label_active = nk_rgb(95, 95, 95);
		ctx.style.combo.border = 1;
		ctx.style.combo.rounding = 1;

		/* combo button */
		ctx.style.combo.button.normal = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.combo.button.hover = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.combo.button.active = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.combo.button.text_background = nk_rgb(216, 216, 216);
		ctx.style.combo.button.text_normal = nk_rgb(95, 95, 95);
		ctx.style.combo.button.text_hover = nk_rgb(95, 95, 95);
		ctx.style.combo.button.text_active = nk_rgb(95, 95, 95);

		/* property */
		ctx.style.property.normal = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.property.hover = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.property.active = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.property.border_color = nk_rgb(81, 81, 81);
		ctx.style.property.label_normal = nk_rgb(95, 95, 95);
		ctx.style.property.label_hover = nk_rgb(95, 95, 95);
		ctx.style.property.label_active = nk_rgb(95, 95, 95);
		ctx.style.property.sym_left = NK_SYMBOL_TRIANGLE_LEFT;
		ctx.style.property.sym_right = NK_SYMBOL_TRIANGLE_RIGHT;
		ctx.style.property.rounding = 10;
		ctx.style.property.border = 1;

		/* edit */
		ctx.style.edit.normal = nk_style_item_color(nk_rgb(240, 240, 240));
		ctx.style.edit.hover = nk_style_item_color(nk_rgb(240, 240, 240));
		ctx.style.edit.active = nk_style_item_color(nk_rgb(240, 240, 240));
		ctx.style.edit.border_color = nk_rgb(62, 62, 62);
		ctx.style.edit.cursor_normal = nk_rgb(99, 202, 255);
		ctx.style.edit.cursor_hover = nk_rgb(99, 202, 255);
		ctx.style.edit.cursor_text_normal = nk_rgb(95, 95, 95);
		ctx.style.edit.cursor_text_hover = nk_rgb(95, 95, 95);
		ctx.style.edit.text_normal = nk_rgb(95, 95, 95);
		ctx.style.edit.text_hover = nk_rgb(95, 95, 95);
		ctx.style.edit.text_active = nk_rgb(95, 95, 95);
		ctx.style.edit.selected_normal = nk_rgb(99, 202, 255);
		ctx.style.edit.selected_hover = nk_rgb(99, 202, 255);
		ctx.style.edit.selected_text_normal = nk_rgb(95, 95, 95);
		ctx.style.edit.selected_text_hover = nk_rgb(95, 95, 95);
		ctx.style.edit.border = 1;
		ctx.style.edit.rounding = 2;

		/* property buttons */
		ctx.style.property.dec_button.normal = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.dec_button.hover = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.dec_button.active = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.dec_button.text_background = nk_rgba(0, 0, 0, 0);
		ctx.style.property.dec_button.text_normal = nk_rgb(95, 95, 95);
		ctx.style.property.dec_button.text_hover = nk_rgb(95, 95, 95);
		ctx.style.property.dec_button.text_active = nk_rgb(95, 95, 95);
		ctx.style.property.inc_button = ctx.style.property.dec_button;

		/* property edit */
		ctx.style.property.edit.normal = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.edit.hover = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.edit.active = nk_style_item_color(
				nk_rgb(216, 216, 216));
		ctx.style.property.edit.border_color = nk_rgba(0, 0, 0, 0);
		ctx.style.property.edit.cursor_normal = nk_rgb(95, 95, 95);
		ctx.style.property.edit.cursor_hover = nk_rgb(95, 95, 95);
		ctx.style.property.edit.cursor_text_normal = nk_rgb(216, 216, 216);
		ctx.style.property.edit.cursor_text_hover = nk_rgb(216, 216, 216);
		ctx.style.property.edit.text_normal = nk_rgb(95, 95, 95);
		ctx.style.property.edit.text_hover = nk_rgb(95, 95, 95);
		ctx.style.property.edit.text_active = nk_rgb(95, 95, 95);
		ctx.style.property.edit.selected_normal = nk_rgb(95, 95, 95);
		ctx.style.property.edit.selected_hover = nk_rgb(95, 95, 95);
		ctx.style.property.edit.selected_text_normal = nk_rgb(216, 216, 216);
		ctx.style.property.edit.selected_text_hover = nk_rgb(216, 216, 216);

		/* chart */
		ctx.style.chart.background = nk_style_item_color(nk_rgb(216, 216, 216));
		ctx.style.chart.border_color = nk_rgb(81, 81, 81);
		ctx.style.chart.color = nk_rgb(95, 95, 95);
		ctx.style.chart.selected_color = nk_rgb(255, 0, 0);
		ctx.style.chart.border = 1;
	}

	/* Draw */
	//struct nk_canvas canvas;
	while (1) {/* GUI */
		{ //struct nk_panel layout, tab;
			if (nk_begin(&ctx, "Demo", nk_rect(0, 0, width, height),
					NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
				uint32_t i;
				float id;
				static int slider = 10;
				static int field_len;
				static nk_size prog_value = 60;
				static int current_weapon = 0;
				static char field_buffer[64];
				static float pos;
				static const char *weapons[] = { "Fist", "Pistol", "Shotgun",
						"Plasma", "BFG" };
				const float step = (2 * 3.141592654f) / 32;

				nk_layout_row_static(&ctx, 30, 120, 1);
				if (nk_button_label(&ctx, "button"))
					fprintf(stdout, "button pressed\n");

				nk_layout_row_dynamic(&ctx, 20, 1);
				nk_label(&ctx, "Label", NK_TEXT_LEFT);
				nk_layout_row_dynamic(&ctx, 30, 2);
				nk_check_label(&ctx, "inactive", 0);
				nk_check_label(&ctx, "active", 1);
				nk_option_label(&ctx, "active", 1);
				nk_option_label(&ctx, "inactive", 0);

				nk_layout_row_dynamic(&ctx, 30, 1);
				nk_slider_int(&ctx, 0, &slider, 16, 1);
				nk_layout_row_dynamic(&ctx, 20, 1);
				nk_progress(&ctx, &prog_value, 100, NK_MODIFIABLE);

				nk_layout_row_dynamic(&ctx, 25, 1);
				nk_edit_string(&ctx, NK_EDIT_FIELD, field_buffer, &field_len,
						64, nk_filter_default);
				nk_property_float(&ctx, "#X:", -1024.0f, &pos, 1024.0f, 1, 1);
				current_weapon = nk_combo(&ctx, weapons, NK_LEN(weapons),
						current_weapon, 25,
						nk_vec2(nk_widget_width(&ctx), 200));

				nk_layout_row_dynamic(&ctx, 100, 1);
				if (nk_chart_begin_colored(&ctx, NK_CHART_LINES,
						nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
					nk_chart_add_slot_colored(&ctx, NK_CHART_LINES,
							nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f,
							1.0f);
					nk_chart_add_slot_colored(&ctx, NK_CHART_LINES,
							nk_rgb(0, 255, 0), nk_rgb(0, 150, 0), 32, -1.0f,
							1.0f);
					for (id = 0, i = 0; i < 32; ++i) {
						nk_chart_push_slot(&ctx, (float) fabs(sin(id)), 0);
						nk_chart_push_slot(&ctx, (float) cos(id), 1);
						nk_chart_push_slot(&ctx, (float) sin(id), 2);
						id += step;
					}
				}
				nk_chart_end(&ctx);

				nk_layout_row_dynamic(&ctx, height, 1);
				nk_layout_row_dynamic(&ctx, 20, 1);
				nk_layout_row_dynamic(&ctx, 20, 1);
			}
			nk_end(&ctx);
		}

		/* Draw each element */
		draw(fb_info, &ctx, width, height);
	}
	nk_free(&ctx);
	nk_buffer_free(&device.cmds);

	printf("\nEnd of program.\nIf you see it then something goes wrong.\n");
	return 0;

}
