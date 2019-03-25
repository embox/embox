/**
 * @file fps.c
 * @brief Simple library for counting frames per second
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 11.03.2019
 */
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <drivers/video/fb.h>
#include <drivers/video/fb_overlay.h>
#include <drivers/video/font.h>
#include <util/log.h>

#include <lib/fps.h>

#define FPS_MSG_LEN 128

static const char fps_default_format_string[] = "Embox FPS=%d";
static const char *fps_format_string = fps_default_format_string;

/**
 * @brief Set format for printed message.
 *
 * @param format: Format string. Should contain '%d' or another specifier to
 *                represent FPS value.
 */
void fps_set_format(const char *format) {
	if (format == NULL) {
		log_error("Passing NULL string as argument, so use default format");
		format = fps_default_format_string;
	}

	fps_format_string = format;
}

static uint8_t *fps_sw_base = 0;
void fps_set_base(struct fb_info *fb, void *base) {
	/* XXX works only with single FB */
	fps_sw_base = base;
}

/**
 * @brief Print message to frame buffer at the left upper corner
 */
void fps_print(struct fb_info *fb) {
	static int fps = -1;
	static int counter = 0;

	char msg_buf[FPS_MSG_LEN];
	char *msg;
	int line;
	static struct timespec time_prev;
	struct timespec time;

	assert(fb);

	memset(msg_buf, 0, sizeof(msg_buf));

	clock_gettime(CLOCK_REALTIME, &time);

	counter++;

	if (time.tv_sec != time_prev.tv_sec) {
		fps = counter / (time.tv_sec - time_prev.tv_sec);
		fps = fps > 999 ? 999 : fps;

		if (fps < 0) {
			log_error("Negative FPS");
		}
		time_prev = time;
		counter = 0;
	}

	snprintf(msg_buf, sizeof(msg_buf), fps_format_string, fps);

	fb_overlay_init(fb, fps_sw_base != 0 ? (void *) fps_sw_base : (void *) fb->screen_base);

	msg = msg_buf;
	line = 0;
	while (*msg) {
		fb_overlay_put_line(0, line, msg);

		while (*msg && *msg != '\n') {
			msg++;
		}

		if (*msg) {
			msg++;
		}

		line++;
	}
}

/**
 * @brief Create second buffer for double buffering
 */
void *fps_enable_swap(struct fb_info *fb) {
	size_t screen_sz;

	assert(fb);
	screen_sz = fb->var.xres * fb->var.yres *
		fb->var.bits_per_pixel / 8;

	if (fps_sw_base != 0) {
		log_error("Can't initialize second frame buffer");
		return fps_sw_base;
	}

	if (0 == (fps_sw_base = malloc(screen_sz))) {
		log_error("Failed to allocate buffer");
		return 0;
	}

	memset(fps_sw_base, 0, screen_sz);

	return fps_sw_base;
}

/**
 * @brief Copy temporary buffer to actual hw frame buffer
 */
int fps_swap(struct fb_info *fb) {
	assert(fps_sw_base);

	memcpy(fb->screen_base,
		fps_sw_base,
		fb->var.xres * fb->var.yres * fb->var.bits_per_pixel / 8);
	return 0;
}
