#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>
#include <drivers/video/fb_overlay.h>
#include <lib/fps.h>
#include <kernel/time/ktime.h>
#include <mem/vmem.h>

#define MAX_QUADS 1024
#define EPS 0.001
#define USE_TRACE 0
#define NEAR 0
#define FAR 1
#define FLIP 0

/* pipe_*_state structs */
#include "pipe/p_state.h"
/* pipe_context */
#include "pipe/p_context.h"
/* pipe_screen */
#include "pipe/p_screen.h"
/* PIPE_* */
#include "pipe/p_defines.h"
/* TGSI_SEMANTIC_{POSITION|GENERIC} */
#include "pipe/p_shader_tokens.h"
/* pipe_buffer_* helpers */
#include "util/u_inlines.h"

/* constant state object helper */
#include "cso_cache/cso_context.h"

/* u_sampler_view_default_template */
#include "util/u_sampler.h"
/* debug_dump_surface_bmp */
#include "util/u_debug_image.h"
/* util_draw_vertex_buffer helper */
#include "util/u_draw_quad.h"
/* FREE & CALLOC_STRUCT */
#include "util/u_memory.h"
/* util_make_[fragment|vertex]_passthrough_shader */
#include "util/u_simple_shaders.h"
/* to get a hardware pipe driver */
#include "pipe-loader/pipe_loader.h"

#include "util/u_debug_image.h"
#include "util/u_format.h"
#include "util/u_inlines.h"
#include "util/u_memory.h"
#include "util/u_string.h"
#include "util/u_surface.h"
#include "util/u_tile.h"

#include <tgsi/tgsi_text.h> /* For compiling custom TGSI shader */

struct program {
	struct pipe_loader_device *dev;
	struct pipe_screen *screen;
	struct pipe_context *pipe;
	struct cso_context *cso;

	struct pipe_blend_state blend;
	struct pipe_depth_stencil_alpha_state depthstencil;
	struct pipe_rasterizer_state rasterizer;
	struct pipe_sampler_state sampler;
	struct pipe_viewport_state viewport;
	struct pipe_framebuffer_state framebuffer;
	struct pipe_vertex_element velem[2];

	void *vs;
	void *fs;

	union pipe_color_union clear_color;

	struct pipe_resource *vbuf;
	struct pipe_resource *target;
	struct pipe_resource *tex;
	struct pipe_sampler_view *view;

	int frame_width;
	int frame_height;

	float quad_sz;

	int quads;

	bool animated;
};

static float vertices[4][2][4] = {
	{
		{ 0.9f, 0.9f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f }
	},
	{
		{ -0.9f, 0.9f, 0.0f, 1.0f },
		{  0.0f, 1.0f, 0.0f, 1.0f }
	},
	{
		{ -0.9f, -0.9f, 0.0f, 1.0f },
		{  0.0f,  0.0f, 1.0f, 1.0f }
	},
	{
		{ 0.9f, -0.9f, 0.0f, 1.0f },
		{ 1.0f,  0.0f, 1.0f, 1.0f }
	}
};

static uint8_t texture[256 * 256 * 2];

static void init_prog(struct program *p)
{
	struct pipe_surface surf_tmpl;
	int ret;

	struct fb_info *mesa_fbi = fb_lookup(0);
	p->frame_width = mesa_fbi->var.xres;
	p->frame_height = mesa_fbi->var.yres;

	/* find a hardware device */
	ret = pipe_loader_probe(&p->dev, 1);
	assert(ret);

	/* init a pipe screen */
	p->screen = pipe_loader_create_screen(p->dev);
	assert(p->screen);

	/* create the pipe driver context and cso context */
	p->pipe = p->screen->context_create(p->screen, NULL, 0);
	p->cso = cso_create_context(p->pipe, 0);

	/* set clear color */
	p->clear_color.f[0] = 0.3;
	p->clear_color.f[1] = 0.3;
	p->clear_color.f[2] = 0.3;
	p->clear_color.f[3] = 1.0;

	/* vertex buffer */
	{
		p->vbuf = pipe_buffer_create(p->screen, PIPE_BIND_VERTEX_BUFFER,
				PIPE_USAGE_DEFAULT, sizeof(vertices));
		pipe_buffer_write(p->pipe, p->vbuf, 0, sizeof(vertices), vertices);
	}

	/* render target texture */
	{
		struct pipe_resource tmplt;
		memset(&tmplt, 0, sizeof(tmplt));
		tmplt.target = PIPE_TEXTURE_2D;
		tmplt.format = PIPE_FORMAT_B5G6R5_UNORM; /* All drivers support this */
		tmplt.width0 = p->frame_width;
		tmplt.height0 = p->frame_height;
		tmplt.depth0 = 1;
		tmplt.array_size = 1;
		tmplt.last_level = 0;
		tmplt.bind = PIPE_BIND_RENDER_TARGET;

		p->target = p->screen->resource_create(p->screen, &tmplt);
	}

	/* sampler texture */
	{
		uint16_t *ptr;
		struct pipe_transfer *t;
		struct pipe_resource t_tmplt;
		struct pipe_sampler_view v_tmplt;
		struct pipe_box box;
		int fd = open("embox_logo256x256.tex", 0);

		read(fd, texture, sizeof(texture));

		close(fd);

		memset(&t_tmplt, 0, sizeof(t_tmplt));
		t_tmplt.target = PIPE_TEXTURE_2D;
		t_tmplt.format = PIPE_FORMAT_B5G6R5_UNORM; /* All drivers support this */
		t_tmplt.width0 = 256;
		t_tmplt.height0 = 256;
		t_tmplt.depth0 = 1;
		t_tmplt.array_size = 1;
		t_tmplt.last_level = 0;
		t_tmplt.bind = PIPE_BIND_RENDER_TARGET;

		p->tex = p->screen->resource_create(p->screen, &t_tmplt);

		memset(&box, 0, sizeof(box));
		box.width = 256;
		box.height = 256;
		box.depth = 1;

		ptr = p->pipe->transfer_map(p->pipe, p->tex, 0, PIPE_TRANSFER_WRITE, &box, &t);

		memcpy(ptr, texture, sizeof(texture));
		p->pipe->transfer_unmap(p->pipe, t);

		u_sampler_view_default_template(&v_tmplt, p->tex, p->tex->format);

		p->view = p->pipe->create_sampler_view(p->pipe, p->tex, &v_tmplt);
	}

	/* disabled blending/masking */
	memset(&p->blend, 0, sizeof(p->blend));
	p->blend.rt[0].colormask = PIPE_MASK_RGBA;

	/* no-op depth/stencil/alpha */
	memset(&p->depthstencil, 0, sizeof(p->depthstencil));

	/* rasterizer */
	memset(&p->rasterizer, 0, sizeof(p->rasterizer));
	p->rasterizer.cull_face = PIPE_FACE_NONE;
	p->rasterizer.half_pixel_center = 1;
	p->rasterizer.bottom_edge_rule = 1;
	p->rasterizer.depth_clip = 1;

	/* sampler */
	memset(&p->sampler, 0, sizeof(p->sampler));
	p->sampler.wrap_s = PIPE_TEX_WRAP_CLAMP_TO_EDGE;
	p->sampler.wrap_t = PIPE_TEX_WRAP_CLAMP_TO_EDGE;
	p->sampler.wrap_r = PIPE_TEX_WRAP_CLAMP_TO_EDGE;
	p->sampler.min_mip_filter = PIPE_TEX_MIPFILTER_NONE;
	p->sampler.min_img_filter = PIPE_TEX_MIPFILTER_LINEAR;
	p->sampler.mag_img_filter = PIPE_TEX_MIPFILTER_LINEAR;
	p->sampler.normalized_coords = 1;

	surf_tmpl.format = PIPE_FORMAT_B5G6R5_UNORM; /* All drivers support this */
	surf_tmpl.u.tex.level = 0;
	surf_tmpl.u.tex.first_layer = 0;
	surf_tmpl.u.tex.last_layer = 0;
	/* drawing destination */
	memset(&p->framebuffer, 0, sizeof(p->framebuffer));
	p->framebuffer.width = p->frame_width;
	p->framebuffer.height = p->frame_height;
	p->framebuffer.nr_cbufs = 1;
	p->framebuffer.cbufs[0] = p->pipe->create_surface(p->pipe, p->target, &surf_tmpl);

	/* viewport, depth isn't really needed */
	{
		float x = 0;
		float y = 0;
		float z = NEAR;
		float half_width = (float)p->frame_width / 2.0f;
		float half_height = (float)p->frame_height / 2.0f;
		float half_depth = ((float)FAR - (float)NEAR) / 2.0f;
		float scale, bias;

		if (FLIP) {
			scale = -1.0f;
			bias = (float)p->frame_height;
		} else {
			scale = 1.0f;
			bias = 0.0f;
		}

		p->viewport.scale[0] = half_width;
		p->viewport.scale[1] = half_height * scale;
		p->viewport.scale[2] = half_depth;

		p->viewport.translate[0] = half_width + x;
		p->viewport.translate[1] = (half_height + y) * scale + bias;
		p->viewport.translate[2] = half_depth + z;
	}

	/* vertex elements state */
	memset(p->velem, 0, sizeof(p->velem));
	p->velem[0].src_offset = 0 * 4 * sizeof(float); /* offset 0, first element */
	p->velem[0].instance_divisor = 0;
	p->velem[0].src_format = PIPE_FORMAT_R32G32B32A32_FLOAT;

	p->velem[1].src_offset = 1 * 4 * sizeof(float); /* offset 16, second element */
	p->velem[1].instance_divisor = 0;
	p->velem[1].vertex_buffer_index = 0;
	p->velem[1].src_format = PIPE_FORMAT_R32G32B32A32_FLOAT;

	/* vertex shader */
	{
		const enum tgsi_semantic semantic_names[] =
		{ TGSI_SEMANTIC_POSITION, TGSI_SEMANTIC_GENERIC };
		const uint semantic_indexes[] = { 0, 0 };
		p->vs = util_make_vertex_passthrough_shader(p->pipe, 2, semantic_names, semantic_indexes, FALSE);
	}

	/* fragment shader */
	{
#if OPTION_GET(BOOLEAN, saturate_texture)
		static char text[] =
			"FRAG\n"
			"DCL IN[0], GENERIC[0], LINEAR\n"
			"DCL OUT[0], COLOR\n"
			"DCL SAMP[0]\n"
			"DCL SVIEW[0], 2D, FLOAT\n"
			"DCL TEMP[0]\n"
			"  0: TEX TEMP[0], IN[0], SAMP[0], 2D\n"
			"  1: ADD TEMP[0], TEMP[0], TEMP[0]\n"
			"  1: MOV OUT[0], TEMP[0]\n"
			"  2: END\n";

		struct tgsi_token tokens[1024];
		struct pipe_shader_state state;

		if (!tgsi_text_translate(text, tokens, ARRAY_SIZE(tokens))) {
			printf("Failed to compile fragment shader!\n");
			return;
		}

		memset(&state, 0, sizeof state);
		state.tokens = tokens;
		p->fs = p->pipe->create_fs_state(p->pipe, &state);
#else /* Saturate texture */
		p->fs = util_make_fragment_tex_shader(p->pipe, TGSI_TEXTURE_2D,
				TGSI_INTERPOLATE_LINEAR,
				TGSI_RETURN_TYPE_FLOAT,
				TGSI_RETURN_TYPE_FLOAT, false,
				false);
#endif
	}
}

static void close_prog(struct program *p)
{
	cso_destroy_context(p->cso);

	p->pipe->delete_vs_state(p->pipe, p->vs);
	p->pipe->delete_fs_state(p->pipe, p->fs);

	pipe_surface_reference(&p->framebuffer.cbufs[0], NULL);
	pipe_sampler_view_reference(&p->view, NULL);
	pipe_resource_reference(&p->target, NULL);
	pipe_resource_reference(&p->tex, NULL);
	pipe_resource_reference(&p->vbuf, NULL);

	p->pipe->destroy(p->pipe);
	p->screen->destroy(p->screen);
	pipe_loader_release(&p->dev, 1);

	FREE(p);
}

void dcache_flush(const void *p, size_t size);
static void draw(struct program *p) {
	const struct pipe_sampler_state *samplers[] = {&p->sampler};
	int step = 0;
	float theta = 0;
	struct fb_info *mesa_fbi = fb_lookup(0);

	float off[MAX_QUADS][2];
	float quad_sz = p->quad_sz > EPS ? p->quad_sz : .9 / sqrt(p->quads);

	float w = quad_sz;
	float h = quad_sz;
	float quadVerts[4][2] = {{w, h}, {-w, h}, {-w, -h}, {w, -h}};
	void *sw_base[2] = { NULL, NULL };

	struct pipe_transfer *transfer[2];
	struct pipe_surface *surface = p->framebuffer.cbufs[0];
	struct pipe_context *pipe = p->pipe;
	struct pipe_resource *texture = surface->texture;
	void *ptr = 0;
	int first_run = 1, current_id = 0;

	/* set the render target */
	cso_set_framebuffer(p->cso, &p->framebuffer);

	cso_set_blend(p->cso, &p->blend);
	cso_set_depth_stencil_alpha(p->cso, &p->depthstencil);
	cso_set_rasterizer(p->cso, &p->rasterizer);
	cso_set_viewport(p->cso, &p->viewport);

	/* sampler */
	cso_set_samplers(p->cso, PIPE_SHADER_FRAGMENT, 1, samplers);

	/* texture sampler view */
	cso_set_sampler_views(p->cso, PIPE_SHADER_FRAGMENT, 1, &p->view);

	/* shaders */
	cso_set_fragment_shader_handle(p->cso, p->fs);
	cso_set_vertex_shader_handle(p->cso, p->vs);

	/* vertex element data */
	cso_set_vertex_elements(p->cso, 2, p->velem);

	for(int i = 0; i < p->quads; ++i) {
		off[i][0] = ((rand() % 1500) - 750.0f) / 1000.0f;
		off[i][1] = ((rand() % 1500) - 750.0f) / 1000.0f;
	}

	if (p->quads == 1) {
		off[0][0] = off[0][1] = 0.0;
	}

	while (true) {
		theta += 0.01;
		const float c = cos(theta);
		const float s = sin(theta);

		float x[4] = {};
		float y[4] = {};

		for (int i = 0; i < 4; i++) {
			x[i] = c * quadVerts[i][0] - s * quadVerts[i][1];
			y[i] = s * quadVerts[i][0] + c * quadVerts[i][1];
		}

		step++;
		current_id = step % 2;

		for (int i = 0; i < 4; i++) {
			/* Rotate vertices by 'theta' radians */
			float x = cos(theta) * vertices[i][0][0] - sin(theta) * vertices[i][0][1];
			float y = sin(theta) * vertices[i][0][0] + cos(theta) * vertices[i][0][1];

			vertices[i][0][0] = x;
			vertices[i][0][1] = y;

			if (fabs(x) > 2.0 || fabs(y) > 2.0) {
				printf("Memory corrupt! %lf %lf\n", x, y);
			}
		}

		/* clear the render target */
		p->pipe->clear(p->pipe, PIPE_CLEAR_COLOR, &p->clear_color, 0, 0);

		for(int quad = 0; quad < p->quads; quad++) {
			for (int i = 0; i < 4; i++) {
				vertices[i][0][0] = x[i] + off[quad][0];
				vertices[i][0][1] = y[i] + off[quad][1];
			}

			pipe_buffer_write(p->pipe, p->vbuf, 0, sizeof(vertices), vertices);

			util_draw_vertex_buffer(p->pipe, p->cso,
					p->vbuf, 0, 0,
					PIPE_PRIM_QUADS,
					4,  /* verts */
					2); /* attribs/vert */
		}
		p->pipe->flush(p->pipe, NULL, 0);

		ptr = pipe_transfer_map(pipe, texture, surface->u.tex.level,
				surface->u.tex.first_layer, PIPE_TRANSFER_READ,
				0, 0, surface->width, surface->height, &transfer[current_id]);

		sw_base[current_id] = ptr;

		if (sw_base[0] && sw_base[1]) {
			if (first_run) {
				vmem_set_flags(vmem_current_context(),
						(mmu_vaddr_t) sw_base[0],
						p->frame_width * p->frame_height * 2,
						PROT_WRITE | PROT_READ | PROT_NOCACHE);

				vmem_set_flags(vmem_current_context(),
						(mmu_vaddr_t) sw_base[1],
						p->frame_width * p->frame_height * 2,
						PROT_WRITE | PROT_READ | PROT_NOCACHE);

				fps_set_base_frame(mesa_fbi, sw_base[0]);
				fps_set_back_frame(mesa_fbi, sw_base[1]);

				first_run = 0;
			}

			fps_print(mesa_fbi);

			fps_swap(mesa_fbi);

			pipe->transfer_unmap(pipe, transfer[current_id ^ 1]);

			if (!p->animated) {
				return;
			}
		}
	}
}

static void print_help(char **argv) {
	printf("Draw rectangle with a texture.\n"
		"USAGE:\n"
		"\t%s [-ah] [-n NUM]\n", argv[0]);
}

int main(int argc, char** argv) {
	struct program *p = CALLOC_STRUCT(program);
	int opt;

	if (p == NULL) {
		printf("Not enough memory to run test\n");
		return -ENOMEM;
	}

	/* Default parameters */
	p->animated = false;
	p->quads = 1;
	p->quad_sz = 0.0;

	while (-1 != (opt = getopt(argc, argv, "ahn:s:"))) {
		switch (opt) {
		case 'a':
			p->animated = true;
			break;
		case 'n':
			p->quads = atoi(optarg);
			if (p->quads < 0 || p->quads >= MAX_QUADS) {
				printf("Wrong parameter for -n\n");
				return -EINVAL;
			}
			break;
		case 's':
			p->quad_sz = atof(optarg);
			if (p->quad_sz < 0) {
				printf("Quad size should be positive float number\n");
				return -EINVAL;
			}

			break;
		case 'h':
			print_help(argv);
			return 0;
		}
	}

	init_prog(p);
	draw(p);
	close_prog(p);

	return 0;
}
