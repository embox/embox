/* This file provides rendering primitives for embox discribed by nuklear.h */

/* includes from embox */
#include <drivers/console/mpx.h>
#include <drivers/video/font.h>

extern const struct font_desc font_vga_8x8, font_vga_8x16;

#define ABS(a) (((a) < 0) ? -(a) : (a)) 
#define MIN(a, b) ((a < b) ? a : b) 
#define MAX(a, b) ((a > b) ? a : b) 



int nk_color_converter(struct nk_color color){
    int b = (color.b / 8)  & (0x1f);
    int g = ((color.g / 8)<<6) & (0x7e0);
    int r = ((color.r / 8)<<11) & 0xf800;
    return r + g + b;
}

void embox_stroke_line(struct vc *vc, float ax, float ay, float bx, float by,int col, float thickness){

    float th = thickness / 2;

    struct fb_fillrect rect;
    rect.dx = 0;
    rect.dy = 0;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    if (ay > by){
        float t = ay;
        ay = by;
        by = t;
    }
    

    for (int i = ax - th; i <= bx + th; ++i) {
	    for (int j = ay - th; j <= by + th; ++j) {
            rect.dx = i;
            rect.dy = j;
		    fb_fillrect(vc->fb, &rect);
	    }
    }
}
void embox_fill_circle(struct vc *vc, int cx, int cy, int r, int col){
    if (r == 0) return;

    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    for (int i = cx - r; i <= cx + r; ++i){
        for (int j = cy - r; j <= cy + r; ++j){
            if ((i - cx)*(i - cx) + (j - cy) * (j - cy) <= r*r){
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}
void embox_fill_arc(struct vc *vc, float cx, float cy, float r, float a_min, float a_max, int col){
    if (r == 0) return;

    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;


    for (float a = a_min; a < a_max; a = a + 0.001){
        for (int i = 0; i <r; i++){
            rect.dx = cx + i * NK_COS(a);
            rect.dy = cy + i * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }
}
void embox_stroke_arc(struct vc *vc, float cx, float cy, float r, float a_min, float a_max, int col, float thickness){
    if (r == 0) return;

    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;


    for (float a = a_min; a < a_max; a = a + 0.001){
        for (float th = 0; th <= thickness; th = th + 0.1){
            rect.dx = cx + (r - th) * NK_COS(a);
            rect.dy = cy + (r - th) * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }
}
void embox_fill_triangle(struct vc *vc, int ax, int ay, int bx, int by, int cx, int cy, int col){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    int if1, if2, if3;


    for (int i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i){
        for (int j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j){
            /* condition of belonging of the point to the triangle */
            if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
            if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
            if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

            if ( (if1 >= 0 && if2 >= 0 && if3 >= 0) || (if1 <= 0 && if2 <= 0 && if3 <= 0) ){
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}   
void embox_stroke_triangle(struct vc *vc, float ax, float ay, float bx, float by, float cx, float cy, int col, float thickness){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    float if1, if2, if3, if4, if5, if6;
    float th = thickness;
    /* ox, oy - center then do scale*/
    float ox = (ax + bx + cx) / 3
         ,oy = (ay + by + cy) / 3
         ,ax2 = ox + (ax - ox) * (th - 1) / th
         ,ay2 = oy + (ay - oy) * (th - 1) / th
         ,bx2 = ox + (bx - ox) * (th - 1) / th
         ,by2 = oy + (by - oy) * (th - 1) / th
         ,cx2 = ox + (cx - ox) * (th - 1) / th
         ,cy2 = oy + (cy - oy) * (th - 1) / th;


    for (int i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i){
        for (int j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j){
            /* condition of belonging of the point to the triangle */
            if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
            if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
            if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

            
            if4 = (ax2 - i) * (by2 - ay2) - (bx2 - ax2) * (ay2 - j);
            if5 = (bx2 - i) * (cy2 - by2) - (cx2 - bx2) * (by2 - j);
            if6 = (cx2 - i) * (ay2 - cy2) - (ax2 - cx2) * (cy2 - j);


            if ( ( (if1 >= 0 && if2 >= 0 && if3 >= 0) || (if1 <= 0 && if2 <= 0 && if3 <= 0) )
            && !(  (if4  >= 0 && if5 >= 0 && if6 >= 0) || (if4 <= 0 && if5 <= 0 && if6 <= 0) ) ) {
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}   
void embox_stroke_rect(struct vc *vc, float x, float y, float w, float h, int col, float rounding, float thickness){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    for (float i = x; i <= x + w; i = i + 0.1){
        for (float j = y; j <= y + thickness; j = j + 0.1){
            rect.dx = i;
            rect.dy = j;
            fb_fillrect(vc->fb, &rect);
            
            rect.dx = i;
            rect.dy = j + h - thickness;
            fb_fillrect(vc->fb, &rect);
        }
    }

     for (float j = y; j <= y + h; j = j + 0.1){
         for (float i = x; i <= x + thickness; i = i + 0.1){
            rect.dx = i;
            rect.dy = j;
            fb_fillrect(vc->fb, &rect);
            
            rect.dx = i + h;
            rect.dy = j;
            fb_fillrect(vc->fb, &rect);
         }

    }

}
void embox_stroke_curve(struct vc *vc, int *x, int *y, int col, float thickness){

    float L0, L1, L2, L3;
    float j;
    float th = thickness /2;

    y[1] = (y[0] + y[1]) / 2;
    y[2] = (y[2] + y[3]) / 2;

    /* interpolation Lagrange */
    for (float i = x[0]; i <= x[3]; i = i + 0.001){
        L0 = (i - x[1]) * (i - x[2]) * (i - x[3]) / (x[0] - x[1]) / (x[0] - x[2]) / (x[0] - x[3]);
        L1 = (i - x[0]) * (i - x[2]) * (i - x[3]) / (x[1] - x[0]) / (x[1] - x[2]) / (x[1] - x[3]);
        L2 = (i - x[0]) * (i - x[1]) * (i - x[3]) / (x[2] - x[0]) / (x[2] - x[1]) / (x[2] - x[3]);
        L3 = (i - x[0]) * (i - x[1]) * (i - x[2]) / (x[3] - x[0]) / (x[3] - x[1]) / (x[3] - x[2]);

        j = y[0] * L0 + y[1] * L1 + y[2] * L2 + y[3] * L3;

        embox_fill_circle(vc, i, j, th, col);
    }
} 
void embox_stroke_circle(struct vc *vc, int cx, int cy, int r, int col, float thickness){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

  for (float a = 0; a < 2 * 3.14; a = a + 0.0001){
        for (float th = 0; th <= thickness ; th = th + 0.1){
            rect.dx = cx + (r - th) * NK_COS(a) ;
            rect.dy = cy + (r - th) * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }


}
void embox_add_text(struct vc *vc, int x, int y, int fg_color, int bg_color, const char *text, int len){
    struct fb_image symbol;
    symbol.dx = x;
    symbol.dy = y;
    symbol.width = 8; //t->w
    symbol.height = 16; //t->h
    symbol.fg_color = fg_color;
    symbol.bg_color = bg_color;
    symbol.depth = 1;

    char *cbuf = (char *) text;
    size_t nbyte = len;
    
    while (nbyte--) {
        symbol.data = (char *)(font_vga_8x16.data) + (unsigned char)(*cbuf++)*16; 
        fb_imageblit(vc->fb, &symbol);
        symbol.dx += 8;
    } 

}
void embox_add_image(struct vc *vc, struct nk_image img, int x, int y, int w, int h, int color){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.rop = ROP_COPY;
    
    for (int nY = 0; nY < w; nY++)
    {
	    for (int nX = 0; nX < h; nX++)
	    {
            /* offset in pixet array according to the image format*/
            uint32_t nOffset = (nX + nY * img.w) * 4;

            int nRed = (int)((unsigned char*)(img.handle.ptr))[nOffset+0];
            int nGreen = (int)((unsigned char*)(img.handle.ptr))[nOffset+1];
            int nBlue = (int)((unsigned char*)(img.handle.ptr))[nOffset+2];
            int nAlpha = (int)((unsigned char*)(img.handle.ptr))[nOffset+3];

            rect.color = nk_color_converter(nk_rgba(nRed, nGreen, nBlue, nAlpha));
            rect.dx = x + nX;
            rect.dy = y + nY;
            
            fb_fillrect(vc->fb, &rect);

        }
    } 
}


static void draw( struct vc *vc, struct nk_context *ctx, int width, int height){ 
    /* shouldn't draw when window is off */
    if (!vc->fb) {
        nk_clear(ctx);
        return;
    }

    const struct nk_command *cmd;

    /* iterate over and execute each draw command */
    nk_foreach(cmd, ctx){
        switch (cmd->type) {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *l = (const struct nk_command_line*)cmd;
            embox_stroke_line(vc, l->begin.x, l->begin.y,l->end.x, l->end.y, nk_color_converter(l->color), l->line_thickness);
        } break;
        case NK_COMMAND_CURVE: {
            const struct nk_command_curve *q = (const struct nk_command_curve*)cmd;
            int x[4];
            int y[4];

            x[0] = q->begin.x;
            x[1] = q->ctrl[0].x;
            x[2] = q->ctrl[1].x;
            x[3] = q->end.x;

            y[0] = q->begin.y;
            y[1] = q->ctrl[0].y;
            y[2] = q->ctrl[1].y;
            y[3] = q->end.y;
            

            embox_stroke_curve(vc, x, y, nk_color_converter(q->color), q->line_thickness);
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect*)cmd;
            embox_stroke_rect(vc, r->x, r->y, r->w, r->h, nk_color_converter(r->color), (float)r->rounding, r->line_thickness);
        } break; 
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *r =
            r = (const struct nk_command_rect_filled*)cmd;
            if ((r->x + r->w >= width) && (r->y + r->h >= height))
                continue;
            struct fb_fillrect rect;
            rect.dx = r->x;
            rect.dy = r->y;
            rect.width = r->w;
            rect.height = r->h;
            rect.color = nk_color_converter(r->color);
            rect.rop = 0;
            fb_fillrect(vc->fb, &rect);
            break;
        }break;
        case NK_COMMAND_CIRCLE: {
            const struct nk_command_circle *c = (const struct nk_command_circle*)cmd;
            embox_stroke_circle(vc,(float)c->x + (float)c->w/2, (float)c->y + (float)c->h/2, (float)c->w/2, nk_color_converter(c->color), c->line_thickness);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
            embox_fill_circle(vc, c->x + c->w/2, c->y + c->h/2, c->w/2, nk_color_converter(c->color));
        } break;
        case NK_COMMAND_ARC: {
            const struct nk_command_arc *c = (const struct nk_command_arc*)cmd;
            embox_stroke_arc(vc, c->cx, c->cy, c->r, c->a[0], c->a[1], nk_color_converter(c->color), c->line_thickness);
        } break;
        case NK_COMMAND_ARC_FILLED: {
            const struct nk_command_arc_filled *c = (const struct nk_command_arc_filled*)cmd;
            embox_fill_arc(vc, c->cx, c->cy, c->r, c->a[0], c->a[1], nk_color_converter(c->color));
        } break;
        case NK_COMMAND_TRIANGLE: {
            const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
            embox_stroke_triangle(vc,t->a.x, t->a.y, t->b.x, t->b.y, t->c.x, t->c.y, nk_color_converter(t->color), t->line_thickness);
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
            embox_fill_triangle(vc,t->a.x, t->a.y, t->b.x, t->b.y, t->c.x, t->c.y, nk_color_converter(t->color));
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
            embox_add_text(vc, t->x, t->y, nk_color_converter(t->foreground), nk_color_converter(t->background), t->string, t->length );
        } break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *i = (const struct nk_command_image*)cmd;
            embox_add_image(vc, i->img, i->x, i->y, i->w, i->h, nk_color_converter(i->col));
        } break;

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

        default: break;
        }
    }
    nk_clear(ctx);

    /* delay for remove flickering */
    for (int i = 0; i<100000000; i++){}
}
