/* This file provides an drawing primitives
 example of work nuklear on OS Embox */
#include <stdio.h>
#include <inttypes.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

/* includes for nuklear*/

#define NK_PRIVATE
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "nk_embox_render.h"

struct nk_canvas {
	struct nk_command_buffer *painter;
	struct nk_vec2 item_spacing;
	struct nk_vec2 panel_padding;
	struct nk_style_item window_background;
};

static void canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas,
		nk_flags flags, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		struct nk_color background_color) {
	/* save style properties which will be overwritten */
	canvas->panel_padding = ctx->style.window.padding;
	canvas->item_spacing = ctx->style.window.spacing;
	canvas->window_background = ctx->style.window.fixed_background;

	/* use the complete window space and set background */
	ctx->style.window.spacing = nk_vec2(0, 0);
	ctx->style.window.padding = nk_vec2(0, 0);
	ctx->style.window.fixed_background = nk_style_item_color(background_color);

	/* create/update window and set position + size */
	flags = flags & ~NK_WINDOW_DYNAMIC;
	nk_begin(ctx, "Window", nk_rect(x, y, width, height),
			NK_WINDOW_NO_SCROLLBAR | flags);
	//nk_window_set_bounds(ctx, nk_rect(x, y, width, height));

	/* allocate the complete window space for drawing */
	{
		struct nk_rect total_space;
		total_space = nk_window_get_content_region(ctx);
		nk_layout_row_dynamic(ctx, total_space.h, 1);
		nk_widget(&total_space, ctx);
		canvas->painter = nk_window_get_canvas(ctx);
	}

}

static void canvas_end(struct nk_context *ctx, struct nk_canvas *canvas) {
	nk_end(ctx);
	ctx->style.window.spacing = canvas->panel_padding;
	ctx->style.window.padding = canvas->item_spacing;
	ctx->style.window.fixed_background = canvas->window_background;
}

static float your_text_width_calculation(nk_handle handle, float height,
		const char *text, int len) {
	return 8;
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
	static struct nk_context ctx;
	static struct nk_canvas canvas;

	uint32_t width = 0, height = 0;

	static struct nk_user_font font;
	font.userdata.ptr = (void *) font_vga_8x8.data;
	font.height = font_vga_8x8.height;
	font.width = your_text_width_calculation;
	nk_init_default(&ctx, &font);

	width = fb_info->var.xres;
	height = fb_info->var.yres;

	/* Draw */
	while (1) {
		/* what to draw */
		canvas_begin(&ctx, &canvas, 0, 0, 0, width, height,
				nk_rgb(100, 100, 100));
		{
			canvas.painter->use_clipping = NK_CLIPPING_OFF;

			nk_fill_rect(canvas.painter, nk_rect(15, 15, 140, 140), 5,
					nk_rgb(247, 230, 154));
			nk_fill_rect(canvas.painter, nk_rect(20, 20, 135, 135), 5,
					nk_rgb(188, 174, 118));
			nk_draw_text(canvas.painter, nk_rect(30, 30, 100, 20),
					"Text to draw", 12, &font, nk_rgb(188, 174, 118),
					nk_rgb(0, 0, 0));
			nk_fill_rect(canvas.painter, nk_rect(160, 20, 70, 70), 0,
					nk_rgb(0, 0, 255));
			nk_fill_circle(canvas.painter, nk_rect(20, 160, 60, 60),
					nk_rgb(255, 0, 0));
			nk_fill_triangle(canvas.painter, 160, 160, 230, 160, 195, 220,
					nk_rgb(0, 255, 0));
			nk_fill_arc(canvas.painter, 195, 120, 30, 0,
					3.141592654f * 3.0f / 4.0f, nk_rgb(255, 255, 0));
			nk_stroke_line(canvas.painter, 15, 10, 100, 10, 2.0f,
					nk_rgb(189, 45, 75));
			nk_stroke_rect(canvas.painter, nk_rect(235, 20, 70, 70), 10, 3,
					nk_rgb(0, 0, 255));
			nk_stroke_curve(canvas.painter, 235, 130, 252, 170, 288, 80, 305,
					130, 1, nk_rgb(0, 150, 220));
			nk_stroke_triangle(canvas.painter, 235, 160, 305, 160, 270, 220, 10,
					nk_rgb(255, 0, 143));
			nk_stroke_circle(canvas.painter, nk_rect(90, 160, 60, 60), 2,
					nk_rgb(0, 255, 120));

			{
				struct nk_image im;

				int w, h, format;
				struct nk_rect r;
				im.handle.ptr = stbi_load("SPBGU_logo.png", &w, &h, &format, 0);
				r = nk_rect(320, 10, w, h);
				im.w = w;
				im.h = h;

				im.region[0] = (unsigned short) 0;
				im.region[1] = (unsigned short) 0;
				im.region[2] = (unsigned short) r.w;
				im.region[3] = (unsigned short) r.h;
				printf("load %p, %d, %d, %d\n", im.handle.ptr, w, h, format);

				nk_draw_image(canvas.painter, r, &im, nk_rgb(100, 0, 0));

				stbi_image_free(im.handle.ptr);
			}

		}
		canvas_end(&ctx, &canvas);

		/* Draw each element */
		draw(fb_info, &ctx, width, height);
	}
	nk_free(&ctx);

	printf("\nEnd of program.\nIf you see it then something goes wrong.\n");
	return 0;
}
