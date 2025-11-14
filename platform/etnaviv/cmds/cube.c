/**
 * @file cube.c
 * @brief Draw rotating cube
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 04.06.2018
 */

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <hal/cache.h>
#include <drivers/video/fb.h>
#include <hal/mmu.h>

#include "xf86drm.h"
#include <etnaviv/etnaviv_drmif.h>
#include <etnaviv/etnaviv_drm.h>
#include <mem/vmem.h>
#include "esTransform.h"
#include <drivers/video/fb.h>

#include <etnaviv_xml/cmdstream.xml.h>
#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_3d.xml.h>
#include <etnaviv_xml/common.xml.h>

#include "viv_compat.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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

/* Now load the shader itself */
uint32_t vs[] = {
	0x01831009, 0x00000000, 0x00000000, 0x203fc048,
	0x02031009, 0x00000000, 0x00000000, 0x203fc058,
	0x07841003, 0x39000800, 0x00000050, 0x00000000,
	0x07841002, 0x39001800, 0x00aa0050, 0x00390048,
	0x07841002, 0x39002800, 0x01540050, 0x00390048,
	0x07841002, 0x39003800, 0x01fe0050, 0x00390048,
	0x03851003, 0x29004800, 0x000000d0, 0x00000000,
	0x03851002, 0x29005800, 0x00aa00d0, 0x00290058,
	0x03811002, 0x29006800, 0x015400d0, 0x00290058,
	0x07851003, 0x39007800, 0x00000050, 0x00000000,
	0x07851002, 0x39008800, 0x00aa0050, 0x00390058,
	0x07851002, 0x39009800, 0x01540050, 0x00390058,
	0x07801002, 0x3900a800, 0x01fe0050, 0x00390058,
	0x0401100c, 0x00000000, 0x00000000, 0x003fc008,
	0x03801002, 0x69000800, 0x01fe00c0, 0x00290038,
	0x03831005, 0x29000800, 0x01480040, 0x00000000,
	0x0383100d, 0x00000000, 0x00000000, 0x00000038,
	0x03801003, 0x29000800, 0x014801c0, 0x00000000,
	0x00801005, 0x29001800, 0x01480040, 0x00000000,
	0x0080108f, 0x3fc06800, 0x00000050, 0x203fc068,
	0x03801003, 0x00000800, 0x01480140, 0x00000000,
	0x04001009, 0x00000000, 0x00000000, 0x200000b8,
	0x02041001, 0x2a804800, 0x00000000, 0x003fc048,
	0x02041003, 0x2a804800, 0x00aa05c0, 0x00000002,
};

uint32_t ps[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};

static    struct etna_bo *rt = 0; /* main render target */
static    struct etna_bo *rt_ts = 0; /* tile status for main render target */
static    struct etna_bo *z = 0; /* depth for main render target */
static    struct etna_bo *z_ts = 0; /* depth ts for main render target */
static    struct etna_bo *vtx = 0; /* vertex buffer */
static    struct etna_bo *aux_rt = 0; /* auxiliary render target */
static    struct etna_bo *aux_rt_ts = 0; /* tile status for auxiliary render target */
static    struct etna_bo *bmp = 0;
static    int width;
static    int height;

size_t rt_size;
size_t rt_ts_size;
size_t z_size;
size_t z_ts_size;
size_t aux_rt_size;
size_t aux_rt_ts_size;

size_t vs_size = sizeof(vs);

uint32_t z_physical, bmp_physical;
uint32_t rt_physical, rt_ts_physical, z_ts_physical, aux_rt_ts_physical, aux_rt_physical, vtx_physical;
int padded_width, padded_height;
int bits_per_tile;

/* macro for MASKED() (multiple can be &ed) */
#define VIV_MASKED(NAME, VALUE) (~(NAME ## _MASK | NAME ## __MASK) | ((VALUE)<<(NAME ## __SHIFT)))
/* for boolean bits */
#define VIV_MASKED_BIT(NAME, VALUE) (~(NAME ## _MASK | NAME) | ((VALUE) ? NAME : 0))
/* for inline enum bit fields
 * XXX in principle headergen could simply generate these fields prepackaged
 */
#define VIV_MASKED_INL(NAME, VALUE) (~(NAME ## _MASK | NAME ## __MASK) | (NAME ## _ ## VALUE))

#define ETNA_FLUSH(stream) \
	etna_set_state((stream), VIVS_GL_FLUSH_CACHE, \
			VIVS_GL_FLUSH_CACHE_DEPTH | \
			VIVS_GL_FLUSH_CACHE_COLOR)
#define VIVS_FE_VERTEX_STREAM_CONTROL_VERTEX_STRIDE__SHIFT FE_VERTEX_STREAM_CONTROL_VERTEX_STRIDE__SHIFT
//#define VIVS_FE_VERTEX_ELEMENT_CONFIG_ENDIAN__SHIFT FE_VERTEX_ELEMENT_CONFIG_ENDIAN__SHIFT
int done = 10;

static void draw_cube(void) {
	static struct timespec time_prev;
	struct timespec time;
	static int fps = 0, counter = 0;

	clock_gettime(CLOCK_REALTIME, &time);
	counter++;

	if (time.tv_sec != time_prev.tv_sec) {
		time_prev = time;
		fps = counter > 99 ? 99 : counter;
		counter = 0;
	}

	struct etna_cmd_stream *stream, *cmdPtr;
	stream = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);

	cmdPtr = stream;

	for (int i = 0; i < 10; i++) {
		etna_cmd_stream_emit(cmdPtr, VIV_FE_NOP_HEADER_OP_NOP);
		etna_cmd_stream_emit(cmdPtr, 0);
	}

	etna_set_state(cmdPtr, VIVS_GL_VERTEX_ELEMENT_CONFIG, 0x1);
	etna_set_state(cmdPtr, VIVS_RA_CONTROL, 0x1);

	etna_set_state(cmdPtr, VIVS_PA_W_CLIP_LIMIT, 0x34000001);
	etna_set_state(cmdPtr, VIVS_PA_SYSTEM_MODE, 0x11);
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, ~((3 << 22)));
	etna_set_state(cmdPtr, VIVS_SE_CONFIG, 0x0);
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_set_state(cmdPtr, VIVS_PE_COLOR_FORMAT,
			VIV_MASKED_BIT(VIVS_PE_COLOR_FORMAT_OVERWRITE, 0));
	etna_set_state(cmdPtr, VIVS_PE_ALPHA_CONFIG, /* can & all these together */
			VIV_MASKED_BIT(VIVS_PE_ALPHA_CONFIG_BLEND_ENABLE_COLOR, 0) &
			VIV_MASKED_BIT(VIVS_PE_ALPHA_CONFIG_BLEND_SEPARATE_ALPHA, 0));
	etna_set_state(cmdPtr, VIVS_PE_ALPHA_CONFIG,
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_SRC_FUNC_COLOR, BLEND_FUNC_ONE) &
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_SRC_FUNC_ALPHA, BLEND_FUNC_ONE));
	etna_set_state(cmdPtr, VIVS_PE_ALPHA_CONFIG,
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_DST_FUNC_COLOR, BLEND_FUNC_ZERO) &
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_DST_FUNC_ALPHA, BLEND_FUNC_ZERO));
	etna_set_state(cmdPtr, VIVS_PE_ALPHA_CONFIG,
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_EQ_COLOR, BLEND_EQ_ADD) &
			VIV_MASKED(VIVS_PE_ALPHA_CONFIG_EQ_ALPHA, BLEND_EQ_ADD));
	etna_set_state(cmdPtr, VIVS_PE_ALPHA_BLEND_COLOR,
			(0 << VIVS_PE_ALPHA_BLEND_COLOR_B__SHIFT) |
			(0 << VIVS_PE_ALPHA_BLEND_COLOR_G__SHIFT) |
			(0 << VIVS_PE_ALPHA_BLEND_COLOR_R__SHIFT) |
			(0 << VIVS_PE_ALPHA_BLEND_COLOR_A__SHIFT));

	etna_set_state(cmdPtr, VIVS_PE_ALPHA_OP, VIV_MASKED_BIT(VIVS_PE_ALPHA_OP_ALPHA_TEST, 0));
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_INL(VIVS_PA_CONFIG_CULL_FACE_MODE, OFF));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_WRITE_ENABLE, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_CONFIG, VIV_MASKED(VIVS_PE_STENCIL_CONFIG_REF_FRONT, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_FUNC_FRONT, COMPARE_FUNC_ALWAYS));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_FUNC_BACK, COMPARE_FUNC_ALWAYS));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_CONFIG, VIV_MASKED(VIVS_PE_STENCIL_CONFIG_MASK_FRONT, 0xff));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_CONFIG, 0xffffff7f); //VIV_MASKED(VIVS_PE_STENCIL_CONFIG_WRITE_MASK, 0xff));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_FAIL_FRONT, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_FAIL_BACK, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_DEPTH_FAIL_FRONT, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_DEPTH_FAIL_BACK, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_PASS_FRONT, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_OP, VIV_MASKED(VIVS_PE_STENCIL_OP_PASS_BACK, STENCIL_OP_KEEP));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_PE_COLOR_FORMAT, VIV_MASKED(VIVS_PE_COLOR_FORMAT_COMPONENTS, 0xf));

	etna_set_state(cmdPtr, VIVS_SE_DEPTH_BIAS, 0x0);

	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_INL(VIVS_PA_CONFIG_FILL_MODE, SOLID));
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_INL(VIVS_PA_CONFIG_SHADE_MODEL, SMOOTH));
	etna_set_state(cmdPtr, VIVS_PE_COLOR_FORMAT,
			VIV_MASKED(VIVS_PE_COLOR_FORMAT_FORMAT, RS_FORMAT_X8R8G8B8) &
			VIV_MASKED_BIT(VIVS_PE_COLOR_FORMAT_SUPER_TILED, 1));

	etna_set_state(cmdPtr, VIVS_PE_COLOR_ADDR, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_PE_PIPE_COLOR_ADDR(0), rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_PE_COLOR_STRIDE, padded_width * 4);
	etna_set_state(cmdPtr, VIVS_GL_MULTI_SAMPLE_CONFIG,
			VIV_MASKED_INL(VIVS_GL_MULTI_SAMPLE_CONFIG_MSAA_SAMPLES, NONE) &
			VIV_MASKED(VIVS_GL_MULTI_SAMPLE_CONFIG_MSAA_ENABLES, 0xf) &
			VIV_MASKED(VIVS_GL_MULTI_SAMPLE_CONFIG_UNK12, 0x0) &
			VIV_MASKED(VIVS_GL_MULTI_SAMPLE_CONFIG_UNK16, 0x0)
		      );
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR);
	etna_set_state(cmdPtr, VIVS_PE_COLOR_FORMAT, VIV_MASKED_BIT(VIVS_PE_COLOR_FORMAT_OVERWRITE, 1));
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR);
	etna_set_state(cmdPtr, VIVS_TS_COLOR_CLEAR_VALUE, 0);
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_TS_MEM_CONFIG, VIVS_TS_MEM_CONFIG_COLOR_FAST_CLEAR);

	etna_set_state(cmdPtr, VIVS_SE_DEPTH_SCALE, 0x0);
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG,
			VIV_MASKED_INL(VIVS_PE_DEPTH_CONFIG_DEPTH_FORMAT, D16) &
			VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_SUPER_TILED, 1)
		      );
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_ADDR, z_physical); /* ADDR_C */
	etna_set_state(cmdPtr, VIVS_PE_PIPE_DEPTH_ADDR(0), z_physical); /* ADDR_C */
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_STRIDE, padded_width * 2);
	etna_set_state(cmdPtr, VIVS_PE_STENCIL_CONFIG, VIV_MASKED_INL(VIVS_PE_STENCIL_CONFIG_MODE, DISABLED));
	etna_set_state(cmdPtr, VIVS_PE_HDEPTH_CONTROL, VIVS_PE_HDEPTH_CONTROL_FORMAT_DISABLED);
	etna_set_state_f32(cmdPtr, VIVS_PE_DEPTH_NORMALIZE, 65535.0);
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 0));
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_DEPTH);

	etna_set_state(cmdPtr, VIVS_TS_DEPTH_CLEAR_VALUE, 0xffffffff);
	etna_set_state(cmdPtr, VIVS_TS_DEPTH_STATUS_BASE, z_ts_physical); /* ADDR_D */
	etna_set_state(cmdPtr, VIVS_TS_DEPTH_SURFACE_BASE, z_physical); /* ADDR_C */
	etna_set_state(cmdPtr, VIVS_TS_MEM_CONFIG,
			VIVS_TS_MEM_CONFIG_DEPTH_FAST_CLEAR |
			VIVS_TS_MEM_CONFIG_COLOR_FAST_CLEAR |
			VIVS_TS_MEM_CONFIG_DEPTH_16BPP |
			VIVS_TS_MEM_CONFIG_DEPTH_COMPRESSION);
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_EARLY_Z, 1)); /* flip-flopping once again */

	/* Warm up RS on aux render target */
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_warm_up_rs(cmdPtr, aux_rt_physical, aux_rt_ts_physical);

	/* Phew, now that's one hell of a setup; the serious rendering starts now */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, rt_physical); /* ADDR_A */

	/* ... or so we thought */
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_warm_up_rs(cmdPtr, aux_rt_physical, aux_rt_ts_physical);

	/* maybe now? */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);

	/* nope, not really... */
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_warm_up_rs(cmdPtr, aux_rt_physical, aux_rt_ts_physical);
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, rt_physical); /* ADDR_A */

	/* semaphore time */
	etna_set_state(cmdPtr, VIVS_GL_SEMAPHORE_TOKEN,
			(SYNC_RECIPIENT_RA<<VIVS_GL_SEMAPHORE_TOKEN_FROM__SHIFT)|
			(SYNC_RECIPIENT_PE<<VIVS_GL_SEMAPHORE_TOKEN_TO__SHIFT)
		      );
	etna_set_state(cmdPtr, VIVS_GL_STALL_TOKEN,
			(SYNC_RECIPIENT_RA<<VIVS_GL_STALL_TOKEN_FROM__SHIFT)|
			(SYNC_RECIPIENT_PE<<VIVS_GL_STALL_TOKEN_TO__SHIFT)
		      );

	/* Set up the resolve to clear tile status for main render target
	 * Regard the TS as an image of width 16 with 4 bytes per pixel (64 bytes per row)
	 * XXX need to clear the depth ts too.
	 * */
	etna_set_state(cmdPtr, VIVS_TS_MEM_CONFIG, 0);
	etna_set_state(cmdPtr, VIVS_RS_CONFIG,
			(RS_FORMAT_A8R8G8B8 << VIVS_RS_CONFIG_SOURCE_FORMAT__SHIFT) |
			(RS_FORMAT_A8R8G8B8 << VIVS_RS_CONFIG_DEST_FORMAT__SHIFT)
		      );
	etna_set_state_multi(cmdPtr, VIVS_RS_DITHER(0), 2, (uint32_t[]){0xffffffff, 0xffffffff});
	etna_set_state(cmdPtr, VIVS_RS_DEST_ADDR, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_RS_PIPE_DEST_ADDR(0), rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_RS_DEST_STRIDE, 0x40);
	etna_set_state(cmdPtr, VIVS_RS_WINDOW_SIZE,
			((rt_ts_size/0x40) << VIVS_RS_WINDOW_SIZE_HEIGHT__SHIFT) |
			(16 << VIVS_RS_WINDOW_SIZE_WIDTH__SHIFT));
	etna_set_state(cmdPtr, VIVS_RS_FILL_VALUE(0), 0x55555555);
	etna_set_state(cmdPtr, VIVS_RS_CLEAR_CONTROL,
			VIVS_RS_CLEAR_CONTROL_MODE_ENABLED1 |
			(0xffff << VIVS_RS_CLEAR_CONTROL_BITS__SHIFT));
	etna_set_state(cmdPtr, VIVS_RS_EXTRA_CONFIG,
			0); /* no AA, no endian switch */
	etna_set_state(cmdPtr, VIVS_RS_KICKER,
			0xbeebbeeb);

	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, 0xffff);
	etna_set_state(cmdPtr, VIVS_TS_COLOR_CLEAR_VALUE, 0xff1f1f1f);
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, rt_ts_physical); /* ADDR_B */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_TS_MEM_CONFIG,
			VIVS_TS_MEM_CONFIG_DEPTH_FAST_CLEAR |
			VIVS_TS_MEM_CONFIG_COLOR_FAST_CLEAR |
			VIVS_TS_MEM_CONFIG_DEPTH_16BPP |
			VIVS_TS_MEM_CONFIG_DEPTH_COMPRESSION);
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_INL(VIVS_PA_CONFIG_CULL_FACE_MODE, CCW));
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);

	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_WRITE_ENABLE, 0));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_INL(VIVS_PE_DEPTH_CONFIG_DEPTH_MODE, NONE));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_WRITE_ENABLE, 0));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED(VIVS_PE_DEPTH_CONFIG_DEPTH_FUNC, COMPARE_FUNC_ALWAYS));
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_INL(VIVS_PE_DEPTH_CONFIG_DEPTH_MODE, Z));
	etna_set_state_f32(cmdPtr, VIVS_PE_DEPTH_NEAR, 0.0);
	etna_set_state_f32(cmdPtr, VIVS_PE_DEPTH_FAR, 1.0);
	etna_set_state_f32(cmdPtr, VIVS_PE_DEPTH_NORMALIZE, 65535.0);

	/* set up primitive assembly */
	etna_set_state_f32(cmdPtr, VIVS_PA_VIEWPORT_OFFSET_Z, 0.0);
	etna_set_state_f32(cmdPtr, VIVS_PA_VIEWPORT_SCALE_Z, 1.);
	etna_set_state(cmdPtr, VIVS_PE_DEPTH_CONFIG, VIV_MASKED_BIT(VIVS_PE_DEPTH_CONFIG_ONLY_DEPTH, 0));
	etna_set_state_fixp(cmdPtr, VIVS_PA_VIEWPORT_OFFSET_X, width << 15);
	etna_set_state_fixp(cmdPtr, VIVS_PA_VIEWPORT_OFFSET_Y, (height << 16));
	etna_set_state_fixp(cmdPtr, VIVS_PA_VIEWPORT_SCALE_X, width << 15);
	etna_set_state_fixp(cmdPtr, VIVS_PA_VIEWPORT_SCALE_Y, height << 16);
	etna_set_state_fixp(cmdPtr, VIVS_SE_SCISSOR_LEFT, 0);
	etna_set_state_fixp(cmdPtr, VIVS_SE_SCISSOR_TOP, 0);
	etna_set_state_fixp(cmdPtr, VIVS_SE_SCISSOR_RIGHT, (width << 17) | 5);
	etna_set_state_fixp(cmdPtr, VIVS_SE_SCISSOR_BOTTOM, (height << 17) | 5);

	/* shader setup */
	etna_set_state(cmdPtr, VIVS_VS_END_PC, vs_size / 16);
	etna_set_state_multi(cmdPtr, VIVS_VS_INPUT_COUNT, 3, (uint32_t[]){
			/* VIVS_VS_INPUT_COUNT */ (1<<8) | 3,
			/* VIVS_VS_TEMP_REGISTER_CONTROL */ 6 << VIVS_VS_TEMP_REGISTER_CONTROL_NUM_TEMPS__SHIFT,
			/* VIVS_VS_OUTPUT(0) */ 4});
	// etna_set_state(cmdPtr, VIVS_VS_RANGE, 0x0 + (0x15 << 16));
	etna_set_state(cmdPtr, VIVS_VS_START_PC, 0x0);
	etna_set_state_f32(cmdPtr, VIVS_VS_UNIFORMS(45), 0.5); /* u11.y */
	etna_set_state_f32(cmdPtr, VIVS_VS_UNIFORMS(44), 1.0); /* u11.x */
	etna_set_state_f32(cmdPtr, VIVS_VS_UNIFORMS(27), 0.0); /* u6.w */
	etna_set_state_f32(cmdPtr, VIVS_VS_UNIFORMS(23), 20.0); /* u5.w */
	etna_set_state_f32(cmdPtr, VIVS_VS_UNIFORMS(19), 2.0); /* u4.w */

	etna_set_state_multi(cmdPtr, VIVS_VS_INST_MEM(0), vs_size/4, vs);
	etna_set_state(cmdPtr, VIVS_RA_CONTROL, 0x1);
	etna_set_state_multi(cmdPtr, VIVS_PS_END_PC, 2, (uint32_t[]){
			/* VIVS_PS_END_PC */ 0x1,
			/* VIVS_PS_OUTPUT_REG */ 0x1});
	etna_set_state(cmdPtr, VIVS_PS_START_PC, 0x0);
	etna_set_state(cmdPtr, VIVS_PA_SHADER_ATTRIBUTES(0), 0x200);
	etna_set_state(cmdPtr, VIVS_GL_VARYING_NUM_COMPONENTS,  /* one varying, with four components */
			(4 << VIVS_GL_VARYING_NUM_COMPONENTS_VAR0__SHIFT)
		      );
	etna_set_state_multi(cmdPtr, VIVS_GL_VARYING_COMPONENT_USE(0), 2, (uint32_t[]){ /* one varying, with four components */
			(VARYING_COMPONENT_USE_USED << VIVS_GL_VARYING_COMPONENT_USE_COMP0__SHIFT) |
			(VARYING_COMPONENT_USE_USED << VIVS_GL_VARYING_COMPONENT_USE_COMP1__SHIFT) |
			(VARYING_COMPONENT_USE_USED << VIVS_GL_VARYING_COMPONENT_USE_COMP2__SHIFT) |
			(VARYING_COMPONENT_USE_USED << VIVS_GL_VARYING_COMPONENT_USE_COMP3__SHIFT)
			, 0
			});
	etna_set_state_multi(cmdPtr, VIVS_PS_INST_MEM(0), sizeof(ps)/4, ps);
	etna_set_state(cmdPtr, VIVS_PS_INPUT_COUNT, (31<<8)|2);
	etna_set_state(cmdPtr, VIVS_PS_TEMP_REGISTER_CONTROL,
			(2 << VIVS_PS_TEMP_REGISTER_CONTROL_NUM_TEMPS__SHIFT));
	etna_set_state(cmdPtr, VIVS_PS_CONTROL,
			VIVS_PS_CONTROL_UNK1
		      );
	etna_set_state(cmdPtr, VIVS_PA_ATTRIBUTE_ELEMENT_COUNT, 0x100);
	etna_set_state(cmdPtr, VIVS_GL_VARYING_TOTAL_COMPONENTS,  /* one varying, with four components */
			(4 << VIVS_GL_VARYING_TOTAL_COMPONENTS_NUM__SHIFT)
		      );
	etna_set_state(cmdPtr, VIVS_VS_LOAD_BALANCING, 0xf3f0582);
	etna_set_state(cmdPtr, VIVS_VS_OUTPUT_COUNT, 2);
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_BIT(VIVS_PA_CONFIG_POINT_SIZE_ENABLE, 0));

	/*   Compute transform matrices in the same way as cube egl demo */
	ESMatrix modelview;
	esMatrixLoadIdentity(&modelview);
	esTranslate(&modelview, 0.0f, 0.0f, -8.0f);
	esRotate(&modelview, 45.0f, 1.0f, 0.0f, 0.0f);
	esRotate(&modelview, 45.0f, 0.0f, 1.0f, 0.0f);
	static int frame = 0;
	esRotate(&modelview, 1.0f * frame, 0.0f, 0.0f, 1.0f);
	frame++;

	printf("FRAME=%d (fps=%d)\n", frame, fps);
	float aspect = (float)(height) / (float)(width);

	ESMatrix projection;
	esMatrixLoadIdentity(&projection);
	esFrustum(&projection, -2.8f, +2.8f, -2.8f * aspect, +2.8f * aspect, 6.0f, 10.0f);

	ESMatrix modelviewprojection;
	esMatrixLoadIdentity(&modelviewprojection);
	esMatrixMultiply(&modelviewprojection, &modelview, &projection);

	//printf("trace %d\n", __LINE__);

	ESMatrix inverse, normal; /* compute inverse transpose normal transformation matrix */
	esMatrixInverse3x3(&inverse, &modelview);
	esMatrixTranspose(&normal, &inverse);

	/*    float normal[9];
	      normal[0] = modelview.m[0][0];
	      normal[1] = modelview.m[0][1];
	      normal[2] = modelview.m[0][2];
	      normal[3] = modelview.m[1][0];
	      normal[4] = modelview.m[1][1];
	      normal[5] = modelview.m[1][2];
	      normal[6] = modelview.m[2][0];
	      normal[7] = modelview.m[2][1];
	      normal[8] = modelview.m[2][2]; */

	etna_set_state_multi(cmdPtr, VIVS_VS_UNIFORMS(0), 16, (uint32_t*)&modelviewprojection.m[0][0]);
	etna_set_state_multi(cmdPtr, VIVS_VS_UNIFORMS(16), 3, (uint32_t*)&normal.m[0][0]); /* u4.xyz */
	etna_set_state_multi(cmdPtr, VIVS_VS_UNIFORMS(20), 3, (uint32_t*)&normal.m[1][0]); /* u5.xyz */
	etna_set_state_multi(cmdPtr, VIVS_VS_UNIFORMS(24), 3, (uint32_t*)&normal.m[2][0]); /* u6.xyz */
	etna_set_state_multi(cmdPtr, VIVS_VS_UNIFORMS(28), 16, (uint32_t*)&modelview.m[0][0]);
	etna_set_state(cmdPtr, VIVS_FE_VERTEX_STREAM_BASE_ADDR, vtx_physical); /* ADDR_E */
	etna_set_state(cmdPtr, VIVS_FE_VERTEX_STREAM_CONTROL,
			0x24 << VIVS_FE_VERTEX_STREAM_CONTROL_VERTEX_STRIDE__SHIFT);
	etna_set_state(cmdPtr, VIVS_FE_VERTEX_ELEMENT_CONFIG(0),
			VIVS_FE_VERTEX_ELEMENT_CONFIG_TYPE_FLOAT |
			(ENDIAN_MODE_NO_SWAP << VIVS_FE_VERTEX_ELEMENT_CONFIG_ENDIAN__SHIFT) |
			(0 << VIVS_FE_VERTEX_ELEMENT_CONFIG_STREAM__SHIFT) |
			(3 <<VIVS_FE_VERTEX_ELEMENT_CONFIG_NUM__SHIFT) |
			VIVS_FE_VERTEX_ELEMENT_CONFIG_NORMALIZE_OFF |
			(0x0 << VIVS_FE_VERTEX_ELEMENT_CONFIG_START__SHIFT) |
			(0xc << VIVS_FE_VERTEX_ELEMENT_CONFIG_END__SHIFT));
	etna_set_state(cmdPtr, VIVS_FE_VERTEX_ELEMENT_CONFIG(1),
			VIVS_FE_VERTEX_ELEMENT_CONFIG_TYPE_FLOAT |
			(ENDIAN_MODE_NO_SWAP << VIVS_FE_VERTEX_ELEMENT_CONFIG_ENDIAN__SHIFT) |
			(0 << VIVS_FE_VERTEX_ELEMENT_CONFIG_STREAM__SHIFT) |
			(3 <<VIVS_FE_VERTEX_ELEMENT_CONFIG_NUM__SHIFT) |
			VIVS_FE_VERTEX_ELEMENT_CONFIG_NORMALIZE_OFF |
			(0xc << VIVS_FE_VERTEX_ELEMENT_CONFIG_START__SHIFT) |
			(0x18 << VIVS_FE_VERTEX_ELEMENT_CONFIG_END__SHIFT));
	etna_set_state(cmdPtr, VIVS_FE_VERTEX_ELEMENT_CONFIG(2),
			VIVS_FE_VERTEX_ELEMENT_CONFIG_TYPE_FLOAT |
			(ENDIAN_MODE_NO_SWAP << VIVS_FE_VERTEX_ELEMENT_CONFIG_ENDIAN__SHIFT) |
			VIVS_FE_VERTEX_ELEMENT_CONFIG_NONCONSECUTIVE |
			(0 << VIVS_FE_VERTEX_ELEMENT_CONFIG_STREAM__SHIFT) |
			(3 <<VIVS_FE_VERTEX_ELEMENT_CONFIG_NUM__SHIFT) |
			VIVS_FE_VERTEX_ELEMENT_CONFIG_NORMALIZE_OFF |
			(0x18 << VIVS_FE_VERTEX_ELEMENT_CONFIG_START__SHIFT) |
			(0x24 << VIVS_FE_VERTEX_ELEMENT_CONFIG_END__SHIFT));
	etna_set_state(cmdPtr, VIVS_VS_INPUT(0), 0x20100);
	etna_set_state(cmdPtr, VIVS_PA_CONFIG, VIV_MASKED_BIT(VIVS_PA_CONFIG_POINT_SPRITE_ENABLE, 0));

	for(int prim=0; prim<6; ++prim) { /* this part is repeated 5 times */
		etna_draw_primitives(cmdPtr, PRIMITIVE_TYPE_TRIANGLE_STRIP, prim * 4, 2);
	}

	etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, 0xffff);
	//etna_warm_up_rs(cmdPtr, aux_rt_physical, aux_rt_ts_physical);

	etna_cmd_stream_finish(cmdPtr);
	etna_cmd_stream_del(cmdPtr);

	cmdPtr = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);

        etna_set_state(cmdPtr, VIVS_GL_FLUSH_CACHE, VIVS_GL_FLUSH_CACHE_COLOR | VIVS_GL_FLUSH_CACHE_DEPTH);
        etna_set_state(cmdPtr, VIVS_RS_CONFIG,
                VIVS_RS_CONFIG_SOURCE_FORMAT(RS_FORMAT_X8R8G8B8) |
                VIVS_RS_CONFIG_SOURCE_TILED |
                VIVS_RS_CONFIG_DEST_FORMAT(RS_FORMAT_X8R8G8B8) |
                VIVS_RS_CONFIG_DEST_TILED);
        etna_set_state(cmdPtr, VIVS_RS_SOURCE_STRIDE, (padded_width * 4 * 4) | VIVS_RS_SOURCE_STRIDE_TILING);
        etna_set_state(cmdPtr, VIVS_RS_DEST_STRIDE, (padded_width * 4 * 4) | VIVS_RS_DEST_STRIDE_TILING);
        etna_set_state(cmdPtr, VIVS_RS_DITHER(0), 0xffffffff);
        etna_set_state(cmdPtr, VIVS_RS_DITHER(1), 0xffffffff);
        etna_set_state(cmdPtr, VIVS_RS_CLEAR_CONTROL, VIVS_RS_CLEAR_CONTROL_MODE_DISABLED);
        etna_set_state(cmdPtr, VIVS_RS_EXTRA_CONFIG, 0); /* no AA, no endian switch */
        etna_set_state(cmdPtr, VIVS_RS_SOURCE_ADDR, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_RS_PIPE_SOURCE_ADDR(0), rt_physical);
        etna_set_state(cmdPtr, VIVS_RS_DEST_ADDR, rt_physical); /* ADDR_A */
	etna_set_state(cmdPtr, VIVS_RS_PIPE_DEST_ADDR(0), rt_physical);
        etna_set_state(cmdPtr, VIVS_RS_WINDOW_SIZE,
                VIVS_RS_WINDOW_SIZE_HEIGHT(padded_height) |
                VIVS_RS_WINDOW_SIZE_WIDTH(padded_width));
        etna_set_state(cmdPtr, VIVS_RS_KICKER, 0xbeebbeeb);

        /* Submit second command buffer */
	etna_cmd_stream_finish(cmdPtr);
	etna_cmd_stream_del(cmdPtr);
}

static void copy_from_rt_to_bmp(void) {
	struct etna_cmd_stream *stream;
	stream = etna_cmd_stream_new(_pipe, 0x1000, NULL, NULL);

	etna_set_state(stream, VIVS_GL_FLUSH_CACHE,
			VIVS_GL_FLUSH_CACHE_DEPTH |
			VIVS_GL_FLUSH_CACHE_COLOR);
	etna_set_state(stream, VIVS_RS_CONFIG,
			VIVS_RS_CONFIG_SOURCE_FORMAT(RS_FORMAT_X8R8G8B8) |
			VIVS_RS_CONFIG_DEST_FORMAT(RS_FORMAT_R5G6B5) |
			VIVS_RS_CONFIG_SOURCE_TILED);
	etna_set_state(stream, VIVS_RS_SOURCE_STRIDE,
			VIVS_RS_SOURCE_STRIDE_TILING | (padded_width * 16));
	etna_set_state(stream, VIVS_RS_DEST_STRIDE,
			VIVS_RS_DEST_STRIDE_TILING | (width * 2));

	etna_set_state(stream, VIVS_RS_DITHER(0),
			0xffffffff);
	etna_set_state(stream, VIVS_RS_DITHER(1),
			0xffffffff);

	etna_set_state(stream, VIVS_RS_CLEAR_CONTROL, 0x0);
	etna_set_state(stream, VIVS_RS_EXTRA_CONFIG, 0x0);

	etna_set_state(stream, VIVS_RS_SOURCE_ADDR, rt_physical);
	etna_set_state(stream, VIVS_RS_PIPE_SOURCE_ADDR(0), rt_physical);
	etna_set_state(stream, VIVS_RS_DEST_ADDR, bmp_physical);
	etna_set_state(stream, VIVS_RS_PIPE_DEST_ADDR(0), bmp_physical);

	etna_set_state(stream, VIVS_RS_WINDOW_SIZE,
			VIVS_RS_WINDOW_SIZE_HEIGHT(height) |
			VIVS_RS_WINDOW_SIZE_WIDTH(width));

	etna_set_state(stream, VIVS_RS_KICKER, 0xbeebbeeb);

	dcache_flush(stream->buffer, 4 * stream->offset);

	etna_cmd_stream_finish(stream);
	etna_cmd_stream_del(stream);
}

static void init_buffers(void) {
	void *vtx_logical;

	padded_width = etna_align_up(width, 64);
	padded_height = etna_align_up(height, 64);

	bits_per_tile = 2;
	rt_size = padded_width * padded_height * 4;
	rt_ts_size = etna_align_up((padded_width * padded_height * 4) * bits_per_tile / 0x80, 0x100);
	z_size = padded_width * padded_height * 2;
	z_ts_size = etna_align_up((padded_width * padded_height * 2) * bits_per_tile / 0x80, 0x100);
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

	printf("Buffers:      rt  %p rt_ts  %p         z %p z_tz %p\n"
			"              vtx %p aux_rt %p aux_rt_tx %p  bmp %p\n",
			etna_bo_map(rt),
			etna_bo_map(rt_ts),
			etna_bo_map(z),
			etna_bo_map(z_ts),
			etna_bo_map(vtx),
			etna_bo_map(aux_rt),
			etna_bo_map(aux_rt_ts),
			etna_bo_map(bmp));

	vtx_logical = etna_bo_map(vtx);

	for (int vert = 0; vert < NUM_VERTICES; ++vert) {
		int src_idx = vert * COMPONENTS_PER_VERTEX;
		int dest_idx = vert * COMPONENTS_PER_VERTEX * 3;
		for(int comp=0; comp<COMPONENTS_PER_VERTEX; ++comp) {
			((float*)vtx_logical)[dest_idx+comp+0] = vVertices[src_idx + comp]; /* 0 */
			((float*)vtx_logical)[dest_idx+comp+3] = vNormals[src_idx + comp]; /* 1 */
			((float*)vtx_logical)[dest_idx+comp+6] = vColors[src_idx + comp]; /* 2 */
		}
	}

	rt_physical =		etna_bo_gpu_address(rt);
	rt_ts_physical =	etna_bo_gpu_address(rt_ts);
	z_physical =		etna_bo_gpu_address(z);
	z_ts_physical =		etna_bo_gpu_address(z_ts);
	aux_rt_physical =	etna_bo_gpu_address(aux_rt);
	aux_rt_ts_physical =	etna_bo_gpu_address(aux_rt_ts);
	vtx_physical =		etna_bo_gpu_address(vtx);
	bmp_physical =		etna_bo_gpu_address(bmp);
}

int main(int argc, char *argv[]) {
	size_t frame_size;
	struct etna_gpu *gpu;
	uint32_t *ptr;
	struct fb_info *fb;
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

	fb = fb_lookup(0);

	width = fb->var.xres;
	height = fb->var.yres;
	frame_size = width * height * fb->var.bits_per_pixel / 8;

	printf("Screen: %dx%d (%"PRIu32"bpp)\n", width, height,
			fb->var.bits_per_pixel);

	if ((version = drmGetVersion(fd))) {
		printf("Version: %d.%d.%d\n", version->version_major,
				version->version_minor,
				version->version_patchlevel);
		printf("  Name: %s\n", version->name);
		printf("  Date: %s\n", version->date);
		printf("  Description: %s\n", version->desc);
		drmFreeVersion(version);
	}

	if (!(dev = etna_device_new(fd))) {
		ret = 2;
		printf("Failed to create Etnaviv device\n");
		goto out_close;
	}

	if (!(gpu = etna_gpu_new(dev, 1))) {
		ret = 3;
		printf("Failed to create Etnaviv GPU\n");
		goto out_dev_del;
	}

	if (!(_pipe = etna_pipe_new(gpu, 0))) {
		ret = 4;
		printf("Failed to create Etnaviv pipe\n");
		goto out_gpu_del;
	}

	if (!(bmp = etna_bo_new(dev, frame_size, ETNA_BO_UNCACHED))) {
		fprintf(stderr, "Unable to allocate buffer\n");
		goto out_pipe_del;
	}

	init_buffers();

	while (1) {
		draw_cube();
		copy_from_rt_to_bmp();

		ptr = etna_bo_map(bmp);
		dcache_inval(ptr, height * width * 2);

		memcpy(fb->screen_base, ptr, frame_size);

		dcache_flush(fb->screen_base, frame_size);
	}

out_pipe_del:
	etna_pipe_del(_pipe);
out_gpu_del:
	etna_gpu_del(gpu);
out_dev_del:
	etna_device_del(dev);
out_close:
	close(fd);

	return ret;
}
