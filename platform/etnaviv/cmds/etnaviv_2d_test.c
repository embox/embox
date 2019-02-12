/*
 * Copyright (C) 2014-2015 Etnaviv Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Christian Gmeiner <christian.gmeiner@gmail.com>
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "xf86drm.h"
#include "etnaviv_drmif.h"
#include "etnaviv_drm.h"

#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_2d.xml.h>
#include <etnaviv_xml/state_3d.xml.h>
#include <etnaviv_xml/cmdstream.xml.h>

#include <drivers/video/fb.h>

#include "viv_compat.h"

static void gen_cmd_stream(struct etna_cmd_stream *stream,
		struct etna_bo *bmp, const int width, const int height) {
	int rec;
	static int num_rects = 256;

	etna_set_state(stream, VIVS_DE_SRC_STRIDE, 0);
	etna_set_state(stream, VIVS_DE_SRC_ROTATION_CONFIG, 0);
	etna_set_state(stream, VIVS_DE_SRC_CONFIG, 0);
	etna_set_state(stream, VIVS_DE_SRC_ORIGIN, 0);
	etna_set_state(stream, VIVS_DE_SRC_SIZE, 0);
	etna_set_state(stream, VIVS_DE_SRC_COLOR_BG, 0);
	etna_set_state(stream, VIVS_DE_SRC_COLOR_FG, 0);
	etna_set_state(stream, VIVS_DE_STRETCH_FACTOR_LOW, 0);
	etna_set_state(stream, VIVS_DE_STRETCH_FACTOR_HIGH, 0);
	etna_set_state_from_bo(stream, VIVS_DE_DEST_ADDRESS, bmp);
	etna_set_state(stream, VIVS_DE_DEST_STRIDE, width*4);
	etna_set_state(stream, VIVS_DE_DEST_ROTATION_CONFIG, 0);
	etna_set_state(stream, VIVS_DE_DEST_CONFIG,
			VIVS_DE_DEST_CONFIG_FORMAT(DE_FORMAT_A8R8G8B8) |
			VIVS_DE_DEST_CONFIG_COMMAND_CLEAR |
			VIVS_DE_DEST_CONFIG_SWIZZLE(DE_SWIZZLE_ARGB) |
			VIVS_DE_DEST_CONFIG_TILED_DISABLE |
			VIVS_DE_DEST_CONFIG_MINOR_TILED_DISABLE
			);
	etna_set_state(stream, VIVS_DE_ROP,
			VIVS_DE_ROP_ROP_FG(0xcc) |
			VIVS_DE_ROP_ROP_BG(0xcc) |
			VIVS_DE_ROP_TYPE_ROP4);
	etna_set_state(stream, VIVS_DE_CLIP_TOP_LEFT,
			VIVS_DE_CLIP_TOP_LEFT_X(0) |
			VIVS_DE_CLIP_TOP_LEFT_Y(0)
			);
	etna_set_state(stream, VIVS_DE_CLIP_BOTTOM_RIGHT,
			VIVS_DE_CLIP_BOTTOM_RIGHT_X(width) |
			VIVS_DE_CLIP_BOTTOM_RIGHT_Y(height)
			);
	etna_set_state(stream, VIVS_DE_CONFIG, 0); /* TODO */
	etna_set_state(stream, VIVS_DE_SRC_ORIGIN_FRACTION, 0);
	etna_set_state(stream, VIVS_DE_ALPHA_CONTROL, 0);
	etna_set_state(stream, VIVS_DE_ALPHA_MODES, 0);
	etna_set_state(stream, VIVS_DE_DEST_ROTATION_HEIGHT, 0);
	etna_set_state(stream, VIVS_DE_SRC_ROTATION_HEIGHT, 0);
	etna_set_state(stream, VIVS_DE_ROT_ANGLE, 0);

	/* Clear color PE20 */
	etna_set_state(stream, VIVS_DE_CLEAR_PIXEL_VALUE32, 0xff40ff40);
	/* Clear color PE10 */
	etna_set_state(stream, VIVS_DE_CLEAR_BYTE_MASK, 0xff);
	etna_set_state(stream, VIVS_DE_CLEAR_PIXEL_VALUE_LOW, 0xff40ff40);
	etna_set_state(stream, VIVS_DE_CLEAR_PIXEL_VALUE_HIGH, 0xff40ff40);

	etna_set_state(stream, VIVS_DE_DEST_COLOR_KEY, 0);
	etna_set_state(stream, VIVS_DE_GLOBAL_SRC_COLOR, 0);
	etna_set_state(stream, VIVS_DE_GLOBAL_DEST_COLOR, 0);
	etna_set_state(stream, VIVS_DE_COLOR_MULTIPLY_MODES, 0);
	etna_set_state(stream, VIVS_DE_PE_TRANSPARENCY, 0);
	etna_set_state(stream, VIVS_DE_PE_CONTROL, 0);
	etna_set_state(stream, VIVS_DE_PE_DITHER_LOW, 0xffffffff);
	etna_set_state(stream, VIVS_DE_PE_DITHER_HIGH, 0xffffffff);

	/* Queue DE command */
	etna_cmd_stream_emit(stream,
			VIV_FE_DRAW_2D_HEADER_OP_DRAW_2D |
			VIV_FE_DRAW_2D_HEADER_COUNT(num_rects)
			/* render one rectangle */
		);
	etna_cmd_stream_emit(stream, 0x0); /* rectangles start aligned */

	for (rec = 0; rec < num_rects; ++rec) {
		int x = rec % 16;
		int y = rec / 16;
		etna_cmd_stream_emit(stream,
				VIV_FE_DRAW_2D_TOP_LEFT_X(x * 8) |
				VIV_FE_DRAW_2D_TOP_LEFT_Y(y * 8));
		etna_cmd_stream_emit(stream,
				VIV_FE_DRAW_2D_BOTTOM_RIGHT_X(x * 8 + 4) |
				VIV_FE_DRAW_2D_BOTTOM_RIGHT_Y(y * 8 + 4));
	}
	etna_set_state(stream, 1, 0);
	etna_set_state(stream, 1, 0);
	etna_set_state(stream, 1, 0);

	etna_set_state(stream, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_PE2D);
}

int main(int argc, char *argv[]) {
	const int width = 256;
	const int height = 256;
	const size_t img_size = width * height * 4;
	struct fb_info *fb;
	struct etna_device *dev;
	struct etna_gpu *gpu;
	struct etna_pipe *pipe;
	struct etna_bo *bmp;
	struct etna_cmd_stream *stream;

	uint32_t *img;

	drmVersionPtr version;
	int fd, ret = 0;

	if (argc < 2) {
		printf("Missing DRM file name, assume /dev/card\n");
		fd = open("/dev/card", O_RDWR);
	} else {
		fd = open(argv[1], O_RDWR);
	}

	if (fd < 0) {
		printf("Failed to open DRM file\n");
		return 1;
	}

	if ((version = drmGetVersion(fd))) {
		printf("Version: %d.%d.%d\n", version->version_major,
		       version->version_minor, version->version_patchlevel);
		printf("  Name: %s\n", version->name);
		printf("  Date: %s\n", version->date);
		printf("  Description: %s\n", version->desc);
		drmFreeVersion(version);
	}

	if (!(dev = etna_device_new(fd))) {
		ret = 2;
		printf("Failed to create Etnaviv device\n");
		goto out;
	}

	/* TODO: we assume that core 0 is a 2D capable one */
	if (!(gpu = etna_gpu_new(dev, 0))) {
		ret = 3;
		printf("Failed to create Etnaviv GPU\n");
		goto out_device;
	}

	if (!(pipe = etna_pipe_new(gpu, ETNA_PIPE_2D))) {
		ret = 4;
		printf("Failed to create Etnaviv pipe\n");
		goto out_gpu;
	}

	if (!(bmp = etna_bo_new(dev, img_size, ETNA_BO_UNCACHED))) {
		ret = 5;
		printf("Failed to map Etnaviv BO\n");
		goto out_pipe;
	}

	memset(etna_bo_map(bmp), 0, img_size);

	if (!(stream = etna_cmd_stream_new(pipe, 0x300, NULL, NULL))) {
		ret = 6;
		printf("Failed to create Etnaviv command stream\n");
		goto out_bo;
	}

	/* generate command sequence */
	gen_cmd_stream(stream, bmp, width, height);

	etna_cmd_stream_finish(stream);

	img = etna_bo_map(bmp);
	fb = fb_lookup(0);

	for (int i = 0; i < height; i++) {
		pix_fmt_convert(img, &((uint16_t *) fb->screen_base)[i * 1280],
				width, RGB888, RGB565);
		img += width;
	}

	etna_cmd_stream_del(stream);
out_bo:
	etna_bo_del(bmp);

out_pipe:
	etna_pipe_del(pipe);

out_gpu:
	etna_gpu_del(gpu);

out_device:
	etna_device_del(dev);

out:
	close(fd);

	return ret;
}
