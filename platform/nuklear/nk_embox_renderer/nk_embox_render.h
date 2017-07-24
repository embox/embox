/* This file provides rendering primitives for embox discribed by nuklear.h */


#include <assert.h>
/* includes from embox */
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <util/log.h>


#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)
#define SIGN(a)   ((a > 0) ? 1 : -1)

static inline int rgba_to_device_color(struct fb_info *fb, const struct nk_color *col) {
	assert(fb);
	assert(col);

	uint32_t native_color = 0;
	uint32_t rgba =
		((col->a & 0xFF) << 24) |
		((col->b & 0xFF) << 16) |
		((col->g & 0xFF) << 8) |
		((col->r & 0xFF));

	pix_fmt_convert(&rgba,
			&native_color,
			1,
			RGBA8888,
			fb->var.fmt);

	return native_color;
}

static inline void embox_fill_rect(struct fb_info *fb, int x, int y,
		int w, int h, const struct nk_color *col) {
	assert(fb);
	assert(col);

	struct fb_fillrect rect;

	rect.dx = x;
	rect.dy = y;
	rect.width = w;
	rect.height = h;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	fb_fillrect(fb, &rect);
}

static inline void embox_fill_triangle(struct fb_info *fb, int ax, int ay, int bx, int by, int cx,
		int cy, const struct nk_color *col) {
	struct fb_fillrect rect;
	int i, j;
	int if1, if2, if3;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for (i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i) {
		for (j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j) {
			/* condition of belonging of the point to the triangle */
			if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
			if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
			if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

			if ((if1 >= 0 && if2 >= 0 && if3 >= 0)
					|| (if1 <= 0 && if2 <= 0 && if3 <= 0)) {
				rect.dx = i;
				rect.dy = j;
				fb_fillrect(fb, &rect);
			}
		}
	}
}

static inline void embox_stroke_line(struct fb_info *fb, float ax, float ay, float bx, float by,
		const struct nk_color *col, float thickness) {
	float tg;
	float dx, dy;
	float Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
	float th = thickness / 2;

	if (ax == bx) {
		embox_fill_rect(fb, ax - th, ay, thickness, ABS(ay - by), col);
		return;
	}
	if (ay == by) {
		embox_fill_rect(fb, ax, ay - th, ABS(ax - bx), thickness, col);
		return;
	}
	if (ax > bx) {
		int t = ax;
		ax = bx;
		bx = t;

		t = ay;
		ay = by;
		by = t;
	}

	tg = (bx - ax) / (by - ay);
	dx = sqrt(th * th / (1 + tg * tg));
	dy = ABS(tg) * dx;

	Ax = ax - dx;
	Ay = ay + SIGN(tg) * dy;
	Bx = ax + dx;
	By = ay - SIGN(tg) * dy;
	Cx = bx + dx;
	Cy = by - SIGN(tg) * dy;
	Dx = bx - dx;
	Dy = by + SIGN(tg) * dy;

	embox_fill_triangle(fb, Ax, Ay, Bx, By, Cx, Cy, col);
	embox_fill_triangle(fb, Ax, Ay, Dx, Dy, Cx, Cy, col);
}

static inline void embox_fill_circle(struct fb_info *fb, int cx, int cy,
		int r, const struct nk_color *col) {
	struct fb_fillrect rect;
	int i, j;

	if (r == 0)
		return;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for (i = cx - r; i <= cx + r; ++i) {
		for (j = cy - r; j <= cy + r; ++j) {
			if ((i - cx) * (i - cx) + (j - cy) * (j - cy) <= r * r) {
				rect.dx = i;
				rect.dy = j;
				fb_fillrect(fb, &rect);
			}
		}
	}
}

static inline void embox_fill_arc(struct fb_info *fb, float cx, float cy, float r, float a_min,
		float a_max, const struct nk_color *col) {
	int i;
	float a;
	struct fb_fillrect rect;

	if (r == 0)
		return;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for (a = a_min; a < a_max; a = a + 0.01) {
		for (i = 0; i < r; i++) {
			rect.dx = cx + i * NK_COS(a);
			rect.dy = cy + i * NK_SIN(a);
			fb_fillrect(fb, &rect);
		}
	}
}

static inline void embox_stroke_arc(struct fb_info *fb, float cx, float cy, float r, float a_min,
		float a_max, const struct nk_color *col, float thickness) {
	struct fb_fillrect rect;
	float a;
	float th;

	if (r == 0)
		return;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for ( a = a_min; a < a_max; a = a + 0.01) {
		for ( th = 0; th <= thickness; th = th + 0.1) {
			rect.dx = cx + (r - th) * NK_COS(a);
			rect.dy = cy + (r - th) * NK_SIN(a);
			fb_fillrect(fb, &rect);
		}
	}
}

static inline void embox_stroke_triangle(struct fb_info *fb, float ax, float ay, float bx,
		float by, float cx, float cy, const struct nk_color *col, float thickness) {
	struct fb_fillrect rect;
	int i, j;
	float if1, if2, if3, if4, if5, if6;
	float th = thickness;
	float ox, oy, ax2, ay2, bx2, by2, cx2, cy2;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	/* ox, oy - center then do scale*/
	ox = (ax + bx + cx) / 3;
	oy = (ay + by + cy) / 3;
	ax2 = ox + (ax - ox) * (th - 1) / th;
	ay2 = oy + (ay - oy) * (th - 1) / th;
	bx2 = ox + (bx - ox) * (th - 1) / th;
	by2 = oy + (by - oy) * (th - 1) / th;
	cx2 = ox + (cx - ox) * (th - 1) / th;
	cy2 = oy + (cy - oy) * (th - 1) / th;

	for (i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i) {
		for (j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j) {
			/* condition of belonging of the point to the triangle */
			if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
			if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
			if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

			if4 = (ax2 - i) * (by2 - ay2) - (bx2 - ax2) * (ay2 - j);
			if5 = (bx2 - i) * (cy2 - by2) - (cx2 - bx2) * (by2 - j);
			if6 = (cx2 - i) * (ay2 - cy2) - (ax2 - cx2) * (cy2 - j);

			if (((if1 >= 0 && if2 >= 0 && if3 >= 0)
					|| (if1 <= 0 && if2 <= 0 && if3 <= 0))
					&& !((if4 >= 0 && if5 >= 0 && if6 >= 0)
							|| (if4 <= 0 && if5 <= 0 && if6 <= 0))) {
				rect.dx = i;
				rect.dy = j;
				fb_fillrect(fb, &rect);
			}
		}
	}
}

static inline void embox_stroke_rect(struct fb_info *fb, float x, float y, float w, float h,
		const struct nk_color *col, float rounding, float thickness) {
	uint32_t th = thickness / 2;
	struct fb_fillrect rect;
	int i, j;

	rect.width = thickness;
	rect.height = thickness;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for (i = x + th; i <= x + w - th; i++) {
		rect.dx = i;
		rect.dy = y + th;
		fb_fillrect(fb, &rect);

		rect.dx = i;
		rect.dy = y + h - th;
		fb_fillrect(fb, &rect);

	}
	for (j = y + th; j <= y + h - th; j++) {
		rect.dx = x + th;
		rect.dy = j;
		fb_fillrect(fb, &rect);

		rect.dx = x + w - th;
		rect.dy = j;
		fb_fillrect(fb, &rect);
	}
}

static inline void embox_stroke_curve(struct fb_info *fb, int *x, int *y,
		const struct nk_color *col, float thickness) {
	float L0, L1, L2, L3;
	float j;
	float th = 1; //thickness /2;

	y[1] = (y[0] + y[1]) / 2;
	y[2] = (y[2] + y[3]) / 2;

	/* interpolation Lagrange */
	for (float i = x[0]; i <= x[3]; i = i + 0.1) {
		L0 = (i - x[1]) * (i - x[2]) * (i - x[3]) / (x[0] - x[1])
				/ (x[0] - x[2]) / (x[0] - x[3]);
		L1 = (i - x[0]) * (i - x[2]) * (i - x[3]) / (x[1] - x[0])
				/ (x[1] - x[2]) / (x[1] - x[3]);
		L2 = (i - x[0]) * (i - x[1]) * (i - x[3]) / (x[2] - x[0])
				/ (x[2] - x[1]) / (x[2] - x[3]);
		L3 = (i - x[0]) * (i - x[1]) * (i - x[2]) / (x[3] - x[0])
				/ (x[3] - x[1]) / (x[3] - x[2]);

		j = y[0] * L0 + y[1] * L1 + y[2] * L2 + y[3] * L3;

		embox_fill_circle(fb, i, j, th, col);
	}
}

static inline void embox_stroke_circle(struct fb_info *fb, int cx, int cy, int r,
		const struct nk_color *col, float thickness) {
	struct fb_fillrect rect;

	rect.width = 1;
	rect.height = 1;
	rect.color = rgba_to_device_color(fb, col);
	rect.rop = ROP_COPY;

	for (float a = 0; a < 2 * 3.14; a = a + 0.01) {
		for (float th = 0; th <= thickness; th = th + 0.1) {
			rect.dx = cx + (r - th) * NK_COS(a);
			rect.dy = cy + (r - th) * NK_SIN(a);
			fb_fillrect(fb, &rect);
		}
	}
}

static inline void embox_draw_symbol(struct fb_info *fb, struct fb_image *symbol,
		const struct nk_color *fg_color, const struct nk_color *bg_color) {
	assert(symbol);
	assert(symbol->depth == 1);

	struct fb_fillrect rect = {
		.width = 1,
		.height = 1,
		.rop = ROP_COPY,
		.dx = symbol->dx,
		.dy = symbol->dy,
	};
	int col, n = -1;

	for (int i = 0; i < symbol->height; i++) {
		for (int j = 0; j < symbol->width; j++) {
			n++;
			rect.dx = j + symbol->dx;
			rect.dy = i + symbol->dy;

			col = ((uint8_t *)symbol->data)[n / 8] & (1 << (7 - (n % 8)));
			if (col) {
				if (fg_color->a == 0)
					continue;
				rect.color = rgba_to_device_color(fb, fg_color);
			} else {
				if (bg_color->a == 0)
					continue;
				rect.color = rgba_to_device_color(fb, bg_color);
			}

			fb_fillrect(fb, &rect);
		}
	}
}

static inline void embox_add_text(struct fb_info *fb, struct nk_context *ctx,
		int x, int y, const struct nk_color *fg_color, const struct nk_color *bg_color, const char *text, int len) {
	struct fb_image symbol;
	char *cbuf;
	size_t nbyte;
	const struct nk_user_font *font;
	void *font_data;

	font = ctx->style.font;

	assert(font);
	font_data = font->userdata.ptr;

	symbol.dx = x;
	symbol.dy = y;
	symbol.width = font->width(font->userdata, 0, NULL, 0);
	symbol.height = font->height;
	symbol.depth = 1;

	cbuf = (char *) text;
	nbyte = len;

	while (nbyte--) {
		symbol.data = (char *) (font_data + (*cbuf++) * symbol.width);
		embox_draw_symbol(fb, &symbol, fg_color, bg_color);
		symbol.dx += symbol.width;
	}
}

/* version of embox_add_image with no changes in image.h */
static inline void embox_add_image(struct fb_info *fb, struct nk_image img, int x, int y, int w,
		int h, int color) {
	assert(fb);

	int nY, nX;
	int nOffset = 0;
	uint8_t *pixel;
	int origX, origY;

	struct fb_fillrect rect = {
		.width = 1,
		.height = 1,
		.rop = ROP_COPY,
	};

	nOffset = 0;
	pixel = (void*) img.handle.ptr;

	if (!pixel) {
		log_debug("Trying to draw empty image, abort");
		return;
	}

	for (nY = 0; nY < h; nY++) {
		for (nX = 0; nX < w; nX++) {
			origX = (nX * img.region[2]) / w;
			origY = (nY * img.region[3]) / h;
			nOffset = (origX + img.region[0] +
					(origY + img.region[1]) * img.w) * 4;

			if (pixel[nOffset + 3] < 3)
				continue;

			pix_fmt_convert(&pixel[nOffset],
					&rect.color,
					1,
					RGBA8888,
					fb->var.fmt);

			rect.dx = x + nX;
			rect.dy = y + nY;

			fb_fillrect(fb, &rect);
		}
	}
}

static inline void draw(struct fb_info *fb, struct nk_context *ctx, int width, int height) {
	assert(fb);
	assert(ctx);

	const struct nk_command *cmd;

	/* iterate over and execute each draw command */
	nk_foreach(cmd, ctx)
	{
		switch (cmd->type) {
		case NK_COMMAND_NOP:
			break;
		case NK_COMMAND_LINE: {
			const struct nk_command_line *c =
					(const struct nk_command_line*) cmd;
			embox_stroke_line( fb, c->begin.x, c->begin.y, c->end.x, c->end.y,
					&c->color, c->line_thickness);
		}
			break;
		case NK_COMMAND_CURVE: {
			const struct nk_command_curve *c =
					(const struct nk_command_curve*) cmd;
			int x[4];
			int y[4];

			x[0] = c->begin.x;
			x[1] = c->ctrl[0].x;
			x[2] = c->ctrl[1].x;
			x[3] = c->end.x;

			y[0] = c->begin.y;
			y[1] = c->ctrl[0].y;
			y[2] = c->ctrl[1].y;
			y[3] = c->end.y;

			embox_stroke_curve( fb, x, y, &c->color, c->line_thickness);
		}
			break;
		case NK_COMMAND_RECT: {
			const struct nk_command_rect *c =
					(const struct nk_command_rect*) cmd;
			embox_stroke_rect( fb, c->x, c->y, c->w, c->h, &c->color,
					(float) c->rounding, c->line_thickness);
		}
			break;
		case NK_COMMAND_RECT_FILLED: {
			const struct nk_command_rect_filled *c =
					(const struct nk_command_rect_filled*) cmd;
			embox_fill_rect( fb, c->x, c->y, c->w, c->h, &c->color);
		}
			break;
		case NK_COMMAND_CIRCLE: {
			const struct nk_command_circle *c =
					(const struct nk_command_circle*) cmd;
			embox_stroke_circle( fb, (float) c->x + (float) c->w / 2,
					(float) c->y + (float) c->h / 2, (float) c->w / 2,
					&c->color, c->line_thickness);
		}
			break;
		case NK_COMMAND_CIRCLE_FILLED: {
			const struct nk_command_circle_filled *c =
					(const struct nk_command_circle_filled *) cmd;
			embox_fill_circle( fb, c->x + c->w / 2, c->y + c->h / 2, c->w / 2,
					&c->color);
		}
			break;
		case NK_COMMAND_ARC: {
			const struct nk_command_arc *c = (const struct nk_command_arc*) cmd;
			embox_stroke_arc( fb, c->cx, c->cy, c->r, c->a[0], c->a[1], &c->color,
					c->line_thickness);
		}
			break;
		case NK_COMMAND_ARC_FILLED: {
			const struct nk_command_arc_filled *c =
					(const struct nk_command_arc_filled*) cmd;
			embox_fill_arc( fb, c->cx, c->cy, c->r, c->a[0], c->a[1], &c->color);
		}
			break;
		case NK_COMMAND_TRIANGLE: {
			const struct nk_command_triangle *c =
					(const struct nk_command_triangle*) cmd;
			embox_stroke_triangle( fb, c->a.x, c->a.y, c->b.x, c->b.y, c->c.x,
					c->c.y, &c->color, c->line_thickness);
		}
			break;
		case NK_COMMAND_TRIANGLE_FILLED: {
			const struct nk_command_triangle_filled *c =
					(const struct nk_command_triangle_filled*) cmd;
			embox_fill_triangle( fb, c->a.x, c->a.y, c->b.x, c->b.y, c->c.x,
					c->c.y, &c->color);
		}
			break;
		case NK_COMMAND_TEXT: {
			const struct nk_command_text *c =
					(const struct nk_command_text*) cmd;
			embox_add_text( fb, ctx, c->x, c->y, &c->foreground, &c->background, c->string,
					c->length);
		}
			break;
		case NK_COMMAND_IMAGE: {
			const struct nk_command_image *c =
					(const struct nk_command_image*) cmd;
			int color = rgba_to_device_color( fb, &c->col);
			embox_add_image( fb, c->img, c->x, c->y, c->w, c->h, color);
		}
			break;

			/* unrealized primitives */
			/*
			 case NK_COMMAND_SCISSOR: {
			 const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
			 nk_draw_list_add_clip(&ctx->draw_list, nk_rect(s->x, s->y, s->w, s->h));
			 } break;
			 case NK_COMMAND_POLYGON: {
			 int i;
			 const struct nk_command_polygon*p = (const struct nk_command_polygon*)cmd;
			 for (i = 0; i < p->point_count; ++i) {
			 struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
			 nk_draw_list_path_line_to(&ctx->draw_list, pnt);
			 }
			 nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_CLOSED, p->line_thickness);
			 } break;
			 case NK_COMMAND_POLYGON_FILLED: {
			 int i;
			 const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
			 for (i = 0; i < p->point_count; ++i) {
			 struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
			 nk_draw_list_path_line_to(&ctx->draw_list, pnt);
			 }
			 nk_draw_list_path_fill(&ctx->draw_list, p->color);
			 } break;
			 case NK_COMMAND_POLYLINE: {
			 int i;
			 const struct nk_command_polyline *p = (const struct nk_command_polyline*)cmd;
			 for (i = 0; i < p->point_count; ++i) {
			 struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
			 nk_draw_list_path_line_to(&ctx->draw_list, pnt);
			 }
			 nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_OPEN, p->line_thickness);
			 } break;
			 case NK_COMMAND_RECT_MULTI_COLOR: {
			 const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color*)cmd;
			 nk_draw_list_fill_rect_multi_color(&ctx->draw_list, nk_rect(r->x, r->y, r->w, r->h),
			 r->left, r->top, r->right, r->bottom);
			 } break; */

		default:
			break;
		}
	}
	nk_clear(ctx);

	/* delay for remove flickering */
	for (int i = 0; i < 100000000000; i++) {
	}
}
