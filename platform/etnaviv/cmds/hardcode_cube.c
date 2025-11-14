/*
 * Copyright (C) 2016 Etnaviv Project.
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "viv_compat.h"

#include "xf86drm.h"
#include <etnaviv/etnaviv_drmif.h>
#include <etnaviv/etnaviv_drm.h>

#include <drivers/video/fb.h>

#include <etnaviv_xml/cmdstream.xml.h>
#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_3d.xml.h>
#include <etnaviv_xml/common.xml.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

extern void dcache_inval(const void *p, size_t size);

#include "cube_cmd.h"

#define VERTEX_BUFFER_SIZE 0x60000
static struct etna_device *dev;
static struct etna_pipe *_pipe;

float vVertices[] = {
	// front
	-1.0f, -1.0f, +1.0f, // point blue
	+1.0f, -1.0f, +1.0f, // point magenta
	-1.0f, +1.0f, +1.0f, // point cyan
	+1.0f, +1.0f, +1.0f, // point white
	// back
	+1.0f, -1.0f, -1.0f, // point red
	-1.0f, -1.0f, -1.0f, // point black
	+1.0f, +1.0f, -1.0f, // point yellow
	-1.0f, +1.0f, -1.0f, // point green
	// right
	+1.0f, -1.0f, +1.0f, // point magenta
	+1.0f, -1.0f, -1.0f, // point red
	+1.0f, +1.0f, +1.0f, // point white
	+1.0f, +1.0f, -1.0f, // point yellow
	// left
	-1.0f, -1.0f, -1.0f, // point black
	-1.0f, -1.0f, +1.0f, // point blue
	-1.0f, +1.0f, -1.0f, // point green
	-1.0f, +1.0f, +1.0f, // point cyan
	// top
	-1.0f, +1.0f, +1.0f, // point cyan
	+1.0f, +1.0f, +1.0f, // point white
	-1.0f, +1.0f, -1.0f, // point green
	+1.0f, +1.0f, -1.0f, // point yellow
	// bottom
	-1.0f, -1.0f, -1.0f, // point black
	+1.0f, -1.0f, -1.0f, // point red
	-1.0f, -1.0f, +1.0f, // point blue
	+1.0f, -1.0f, +1.0f  // point magenta
};

float vColors[] = {
	// front
	0.0f,  0.0f,  1.0f, // blue
	1.0f,  0.0f,  1.0f, // magenta
	0.0f,  1.0f,  1.0f, // cyan
	1.0f,  1.0f,  1.0f, // white
	// back
	1.0f,  0.0f,  0.0f, // red
	0.0f,  0.0f,  0.0f, // black
	1.0f,  1.0f,  0.0f, // yellow
	0.0f,  1.0f,  0.0f, // green
	// right
	1.0f,  0.0f,  1.0f, // magenta
	1.0f,  0.0f,  0.0f, // red
	1.0f,  1.0f,  1.0f, // white
	1.0f,  1.0f,  0.0f, // yellow
	// left
	0.0f,  0.0f,  0.0f, // black
	0.0f,  0.0f,  1.0f, // blue
	0.0f,  1.0f,  0.0f, // green
	0.0f,  1.0f,  1.0f, // cyan
	// top
	0.0f,  1.0f,  1.0f, // cyan
	1.0f,  1.0f,  1.0f, // white
	0.0f,  1.0f,  0.0f, // green
	1.0f,  1.0f,  0.0f, // yellow
	// bottom
	0.0f,  0.0f,  0.0f, // black
	1.0f,  0.0f,  0.0f, // red
	0.0f,  0.0f,  1.0f, // blue
	1.0f,  0.0f,  1.0f  // magenta
};

float vNormals[] = {
	// front
	+0.0f, +0.0f, +1.0f, // forward
	+0.0f, +0.0f, +1.0f, // forward
	+0.0f, +0.0f, +1.0f, // forward
	+0.0f, +0.0f, +1.0f, // forward
	// back
	+0.0f, +0.0f, -1.0f, // backbard
	+0.0f, +0.0f, -1.0f, // backbard
	+0.0f, +0.0f, -1.0f, // backbard
	+0.0f, +0.0f, -1.0f, // backbard
	// right
	+1.0f, +0.0f, +0.0f, // right
	+1.0f, +0.0f, +0.0f, // right
	+1.0f, +0.0f, +0.0f, // right
	+1.0f, +0.0f, +0.0f, // right
	// left
	-1.0f, +0.0f, +0.0f, // left
	-1.0f, +0.0f, +0.0f, // left
	-1.0f, +0.0f, +0.0f, // left
	-1.0f, +0.0f, +0.0f, // left
	// top
	+0.0f, +1.0f, +0.0f, // up
	+0.0f, +1.0f, +0.0f, // up
	+0.0f, +1.0f, +0.0f, // up
	+0.0f, +1.0f, +0.0f, // up
	// bottom
	+0.0f, -1.0f, +0.0f, // down
	+0.0f, -1.0f, +0.0f, // down
	+0.0f, -1.0f, +0.0f, // down
	+0.0f, -1.0f, +0.0f  // down
};
#define COMPONENTS_PER_VERTEX (3)
#define NUM_VERTICES (6*4)

static    struct etna_bo *rt = 0; /* main render target */
static    struct etna_bo *rt_ts = 0; /* tile status for main render target */
static    struct etna_bo *z = 0; /* depth for main render target */
static    struct etna_bo *z_ts = 0; /* depth ts for main render target */
static    struct etna_bo *vtx = 0; /* vertex buffer */
static    struct etna_bo *aux_rt = 0; /* auxiliary render target */
static    struct etna_bo *aux_rt_ts = 0; /* tile status for auxiliary render target */
static    int width = 400;
static    int height = 240;

size_t rt_size;
size_t rt_ts_size;
size_t z_size;
size_t z_ts_size;
size_t aux_rt_size;
size_t aux_rt_ts_size;

static void ins_nop(uint32_t *buf, int offt) {
	buf[offt]     = 0x18000000;
	buf[offt + 1] = 0x00000000;
}

static void ins_nopn(uint32_t *buf, int offt, int n) {
	for (int i = 0; i < n; i++) {
		buf[offt + i * 2]     = 0x18000000;
		buf[offt + 1 + i * 2] = 0x00000000;
	}
}

static int add_pipe_ops(uint32_t *buf, int len) {
	static int use_dither = 1;

	for (int i = 0; i < len; i++) {
		/* RS source addr */
		if (buf[i] == 0x08010582) {
			uint32_t arg = buf[i + 1];

			for (int j = len + 1; j >= i + 2; j--) {
				buf[j] = buf[j - 2];
			}

			buf[i + 2] = 0x080105b0; /* RS.PIPE_SOURCE */
			buf[i + 3] = arg;
			i += 3;
			len += 2;

			continue;
		}

		/* RS dest addr */
		if (buf[i] == 0x08010584) {
			uint32_t arg = buf[i + 1];

			for (int j = len + 1; j >= i + 2; j--) {
				buf[j] = buf[j - 2];
			}

			buf[i + 2] = 0x080105b8; /* RS.PIPE_DEST */
			buf[i + 3] = arg;
			i += 3;
			len += 2;

			continue;
		}

		/* PE color addr */
		if (buf[i] == 0x0801050c) {
			uint32_t arg = buf[i + 1];

			for (int j = len + 1; j >= i + 2; j--) {
				buf[j] = buf[j - 2];
			}

			buf[i + 2] = 0x08010518; /* PE.PIPE_COLOR */
			buf[i + 3] = arg;
			i += 3;
			len += 2;

			continue;
		}

		/* PE depth addr */
		if (buf[i] == 0x08010504) {
			uint32_t arg = buf[i + 1];

			for (int j = len + 1; j >= i + 2; j--) {
				buf[j] = buf[j - 2];
			}

			buf[i + 2] = 0x08010520; /* PE.PIPE_DEPTH */
			buf[i + 3] = arg;
			i += 3;
			len += 2;

			continue;
		}

		if (buf[i] == 0x08010590) {
			int n = 6;
			uint32_t tile = 0x55555555;//0xffffffff;//0x55555555;//0x000000f0;
			for (int j = len + n - 1 + (n % 2 ? 0 : 1); j >= i + n - 1; j--) {
				buf[j] = buf[j - n + 1 - (n % 2 ? 0 : 1)];
			}

			buf[i] = 0x08000590 | (((n % 2) ? n : n - 1) << 16);

			for (int j = 1; j <= n; j ++) {
				buf[i + j] = tile;
			}
			if (1) {
			//buf[i + 1] = 0xffff0000;
			//buf[i + 2] = 0xff00ff00;
			//buf[i + 3] = 0xff0000ff;
			buf[i + 4] = 0xffff00ff;

			if (0) buf[i + 2] = tile;
			tile = 0;
			}
			len += n - 1;

			if (!(n % 2)) {
				buf[i + n] = 0x08010590;
				buf[i + n + 1] = tile;
				len++;
				i++;
			}

			i += n;

			continue;
		}

		if (buf[i] == 0x0801058c || buf[i] == 0x0801058d) {
			buf[i + 1] = use_dither * 0xffffffff;
			i++;
			continue;
		}

		if (buf[i] == 0x0802058c) {
			buf[i + 1] = buf[i + 2] = use_dither * 0xffffffff;
			i += 2;
			continue;
		}

		if (buf[i] == 0x0801050b) { /* PE_COLOR_FORMAT */
			//buf[i + 1] |= (1 << 20); // non-supertiled
			i++;
			continue;
		}

		if (buf[i] == 0x08010500) {
			//buf[i + 1] |= (1 << 26); // non-supertiled
			i++;
			continue;
		}

		if (buf[i] == 0x08010583 || buf[i] == 0x08010585) {
			//buf[i + 1] |= (1 << 31); // non-supertiled
			i++;
			continue;
		}

		if (buf[i] == 0x08010e03) { /* FLush cache */
			buf[i + 1] = 3;
		}
	}

	return len;
}

static void wait_tick(void) {
	int t = 0xffffff;
	while(t--);
}

static void stall_tick(void) {
	int cnt = 0;
	while(1) {
		wait_tick();
		printf("cnt=%d\n", cnt++);
	}
}

void nonzero_lookup_raw(void *_ptr, int len, const char *name) {
	int *ptr = _ptr;
	int cnt = 0;
	printf("Look for non-zero elements for %s\n", name ? name : "");
	dcache_inval(ptr, len);
	for (int i = 0; i < len / 4; i++, ptr++) {
		if (*ptr != 0) {
			cnt++;
			printf("%s non-zero %d=%08x\n", name ? name : "", i, *ptr);
			if (cnt > 32) {
				printf("...\n");
				return;
			}
		}
	}
}

void nonzero_lookup(struct etna_bo *stream, int len, const char *name) {
	nonzero_lookup_raw(etna_bo_map(stream), len, name);
}

static void gen_cmd_stream(struct etna_bo *bmp)
{
	uint32_t rt_physical, rt_ts_physical, z_ts_physical, aux_rt_ts_physical, aux_rt_physical, vtx_physical;
	uint32_t z_physical, bmp_physical;
	int padded_width = etna_align_up(width, 64) / 2;
	int padded_height = etna_align_up(height, 64) / 2;


	static struct etna_cmd_stream *stream;
	stream = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);
	printf("stream buf is %p\n", stream->buffer);

	int bits_per_tile = 2;
	rt_size = padded_width * padded_height * 4;
	rt_ts_size = etna_align_up((padded_width * padded_height * 4)*bits_per_tile/0x80, 0x100);
	z_size = padded_width * padded_height * 2;
	z_ts_size = etna_align_up((padded_width * padded_height * 2)*bits_per_tile/0x80, 0x100);
	aux_rt_size = 0x4000;
	aux_rt_ts_size = 0x80 * bits_per_tile;

	/* Allocate buffers */
	if( (rt=etna_bo_new(dev, rt_size, ETNA_BO_UNCACHED))==NULL ||
			(rt_ts=etna_bo_new(dev, rt_ts_size, ETNA_BO_UNCACHED))==NULL ||
			(z=etna_bo_new(dev, z_size, ETNA_BO_UNCACHED))==NULL ||
			(z_ts=etna_bo_new(dev, z_ts_size, ETNA_BO_UNCACHED))==NULL ||
			(vtx=etna_bo_new(dev, VERTEX_BUFFER_SIZE, ETNA_BO_UNCACHED))==NULL ||
			(aux_rt=etna_bo_new(dev, aux_rt_size, ETNA_BO_UNCACHED))==NULL ||
			(aux_rt_ts=etna_bo_new(dev, aux_rt_ts_size, ETNA_BO_UNCACHED))==NULL) {
		fprintf(stderr, "Error allocating video memory\n");
		return;
	}

	void *vtx_logical = etna_bo_map(vtx);

	for (int vert = 0; vert < NUM_VERTICES; ++vert) {
		int src_idx = vert * COMPONENTS_PER_VERTEX;
		int dest_idx = vert * COMPONENTS_PER_VERTEX * 3;
		for(int comp=0; comp<COMPONENTS_PER_VERTEX; ++comp) {
			((float*)vtx_logical)[dest_idx+comp+0] = vVertices[src_idx + comp]; /* 0 */
			((float*)vtx_logical)[dest_idx+comp+3] = vNormals[src_idx + comp]; /* 1 */
			((float*)vtx_logical)[dest_idx+comp+6] = vColors[src_idx + comp]; /* 2 */
		}
	}

	rt_physical =		((int) etna_bo_map(rt))        - 0x10000000 * 1;
	rt_ts_physical =	((int) etna_bo_map(rt_ts))     - 0x10000000 * 1;
	z_physical =		((int) etna_bo_map(z))         - 0x10000000 * 1;
	z_ts_physical =		((int) etna_bo_map(z_ts))      - 0x10000000 * 1;
	aux_rt_physical =	((int) etna_bo_map(aux_rt))    - 0x10000000 * 1;
	aux_rt_ts_physical =	((int) etna_bo_map(aux_rt_ts)) - 0x10000000 * 1;
	vtx_physical =		((int) etna_bo_map(vtx))       - 0x10000000 * 1;
	bmp_physical =		((int) etna_bo_map(bmp))       - 0x10000000 * 1;


	printf("Some buffers:  rt %p  rt_ts %p         z %p z_tz %p\n"
	       "              vtx %p aux_rt %p aux_rt_tx %p  bmp %p\n",
			etna_bo_map(rt),
			etna_bo_map(rt_ts),
			etna_bo_map(z),
			etna_bo_map(z_ts),
			etna_bo_map(vtx),
			etna_bo_map(aux_rt),
			etna_bo_map(aux_rt_ts),
			etna_bo_map(bmp));

	printf("cmdbuf1 sz is %d (%x)\n", ARRAY_SIZE(cmdbuf1), ARRAY_SIZE(cmdbuf1));

	cmdbuf1[87] = cmdbuf1[103] = cmdbuf1[161] = cmdbuf1[189] = cmdbuf1[219] = cmdbuf1[253] = rt_physical;
	cmdbuf1[101] = cmdbuf1[159] = cmdbuf1[187] = cmdbuf1[217] = cmdbuf1[231] = cmdbuf1[251] = rt_ts_physical;
	cmdbuf1[111] = cmdbuf1[129] = z_physical;
	cmdbuf1[127] = z_ts_physical;
	cmdbuf1[197] = cmdbuf1[139] = cmdbuf1[145]
		= cmdbuf1[149] = cmdbuf1[203] = cmdbuf1[167]
		= cmdbuf1[173] = cmdbuf1[177] = cmdbuf1[207] = aux_rt_physical;


	cmdbuf1[137] = cmdbuf1[165] = cmdbuf1[195] = aux_rt_ts_physical;
	cmdbuf1[501] = cmdbuf1[535] = cmdbuf1[569]
		= cmdbuf1[603] = cmdbuf1[637] = cmdbuf1[671] = vtx_physical;

	if (0) { ins_nop(0, 0); ins_nopn(0, 0, 0); stall_tick(); }

	stream->offset = add_pipe_ops(cmdbuf1, 690);
	memcpy(stream->buffer,
			cmdbuf1,
			sizeof(cmdbuf1));

	etna_cmd_stream_finish(stream);

	printf("Finished stream 1\n");
	static struct etna_cmd_stream *stream2;
	stream2 = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);
	printf("stream2 buf is %p\n", stream2->buffer);

	memcpy(	stream2->buffer,
			cmdbuf2,
			sizeof(cmdbuf2));
	stream2->offset = ARRAY_SIZE(cmdbuf2);

	etna_cmd_stream_finish(stream2);

	printf("Finished stream 2\n");

	static struct etna_cmd_stream *stream3;
	stream3 = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);
	printf("stream3 buf is %p\n", stream3->buffer);
	cmdbuf3[0x9] = aux_rt_ts_physical;
	cmdbuf3[0xb] = cmdbuf3[0x11] = cmdbuf3[0x15] = cmdbuf3[25] = cmdbuf3[27] = aux_rt_physical;
	cmdbuf3[0x1f + 4] = rt_ts_physical;
	cmdbuf3[0x21 + 4] = rt_physical;


	stream3->offset = add_pipe_ops(cmdbuf3, 32);

	memcpy(	stream3->buffer,
			cmdbuf3,
			sizeof(cmdbuf3));

	if (0) etna_cmd_stream_finish(stream3);

	printf("Finished stream 3\n");

	static struct etna_cmd_stream *stream4;
	stream4 = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);

	printf("stream4 buf is %p\n", stream4->buffer);
	cmdbuf4[0x19] = rt_physical;
	cmdbuf4[0x1b] = bmp_physical;

	stream4->offset = add_pipe_ops(cmdbuf4, 32);

	memcpy(	stream4->buffer,
			cmdbuf4,
			sizeof(cmdbuf4));

	etna_cmd_stream_finish(stream4);
	printf("Finished stream 4\n");

	etna_cmd_stream_del(stream);
	etna_cmd_stream_del(stream2);
	etna_cmd_stream_del(stream3);
	etna_cmd_stream_del(stream4);
}

int main(int argc, char *argv[]) {
	size_t out_size = width * height * 4;
	struct etna_gpu *gpu;
	struct etna_bo *bmp;
	uint32_t *ptr;
	struct fb_info *fbi;

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

	fbi = fb_lookup(0);

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
	}

	if (!(gpu = etna_gpu_new(dev, 1))) {
		ret = 3;
		printf("Failed to create Etnaviv GPU\n");
	}

	if (!(_pipe = etna_pipe_new(gpu, 0))) {
		ret = 4;
		printf("Failed to create Etnaviv pipe\n");
	}

	if (!(bmp = etna_bo_new(dev, out_size, ETNA_BO_UNCACHED))) {
		fprintf(stderr, "Unable to allocate buffer\n");
	}
	memset(etna_bo_map(bmp), 0, out_size);

	gen_cmd_stream(bmp);
	ptr = etna_bo_map(bmp);
	dcache_inval(ptr, height * width * 4);

	for (int i = 0; i < height; i++) {
		pix_fmt_convert(ptr, &((uint16_t *) fbi->screen_base)[i * 1280],
				width, RGB888, RGB565);
		ptr += width;
	}

	if (0) {
		nonzero_lookup(rt, 16, "rt");
		nonzero_lookup(rt_ts, rt_ts_size, "rt_ts");
		nonzero_lookup(z, z_size, "z");
		nonzero_lookup(z_ts, z_ts_size, "z_ts");
		nonzero_lookup(aux_rt, aux_rt_size, "aux_rt");
		nonzero_lookup(aux_rt_ts, aux_rt_ts_size, "aux_rt_ts");
		nonzero_lookup(bmp, out_size, "bmp");
	}

	return ret;
}
