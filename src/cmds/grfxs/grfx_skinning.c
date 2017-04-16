
/* includes for nuklear*/
#include <stdio.h>
#define NK_PRIVATE
#define NK_API
#define NK_INTERN static
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* includes from fbcon */
#include <drivers/console/mpx.h>
#include <drivers/video/fb.h>


#define ABS(a) (((a) < 0) ? -(a) : (a)) 
#define MIN(a, b) ((a < b) ? a : b) 
#define MAX(a, b) ((a > b) ? a : b) 

struct device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    NK_UINT32 vbo, vao, ebo;
    NK_UINT32 font_tex;
};

struct nk_canvas {
    struct nk_command_buffer *painter;
    struct nk_vec2 item_spacing;
    struct nk_vec2 panel_padding;
    struct nk_style_item window_background;
};

struct media {
    int skin;
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

int nk_color_converter(struct nk_color color){
    int b = (color.b / 8)  & (0x1f);
    int g = ((color.g / 8)<<6) & (0x7e0);
    int r = ((color.r / 8)<<11) & 0xf800;
    return r + g + b;
}

/* rendering primitives*/
void embox_stroke_line(struct vc *vc, float ax, float ay, float bx, float by,int col, float thickness){

    float th = thickness/2;

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
		    //*(((uint16_t*)fb->screen_base) + i * var.xres + j) = ((i * 32 / 200) << 11) + (0x1f & (j * 32 / 200));
            fb_fillrect(vc->fb, &rect);
	    }
    }
}
void embox_fill_circle(struct vc *vc, int cx, int cy, int r, int col){
    struct fb_fillrect rect;
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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
    rect.dx = 0;
    rect.dy = 0;
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


static void device_draw( struct vc *vc, struct device *dev, struct nk_context *ctx, int width, int height)
{
    
    /* shouldn't draw when window is off */
    if (!vc->fb) {
        nk_clear(ctx);
        return;
    }

    const struct nk_command *cmd;
    

    /* iterate over and execute each draw command */
    nk_foreach(cmd, ctx)
    {


        switch (cmd->type) {
        case NK_COMMAND_NOP: break;
    
        // case NK_COMMAND_SCISSOR: {
        //     const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
        //     nk_draw_list_add_clip(&ctx->draw_list, nk_rect(s->x, s->y, s->w, s->h));
        // } break;

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
            const struct nk_command_rect_filled *r;
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
        // case NK_COMMAND_RECT_MULTI_COLOR: {
        //     const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color*)cmd;
        //     nk_draw_list_fill_rect_multi_color(&ctx->draw_list, nk_rect(r->x, r->y, r->w, r->h),
        //         r->left, r->top, r->right, r->bottom);
        // } break;
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
        // case NK_COMMAND_POLYGON: {
        //     int i;
        //     const struct nk_command_polygon*p = (const struct nk_command_polygon*)cmd;
        //     for (i = 0; i < p->point_count; ++i) {
        //         struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
        //         nk_draw_list_path_line_to(&ctx->draw_list, pnt);
        //     }
        //     nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_CLOSED, p->line_thickness);
        // } break;
        // case NK_COMMAND_POLYGON_FILLED: {
        //     int i;
        //     const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
        //     for (i = 0; i < p->point_count; ++i) {
        //         struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
        //         nk_draw_list_path_line_to(&ctx->draw_list, pnt);
        //     }
        //     nk_draw_list_path_fill(&ctx->draw_list, p->color);
        // } break;
        // case NK_COMMAND_POLYLINE: {
        //     int i;
        //     const struct nk_command_polyline *p = (const struct nk_command_polyline*)cmd;
        //     for (i = 0; i < p->point_count; ++i) {
        //         struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
        //         nk_draw_list_path_line_to(&ctx->draw_list, pnt);
        //     }
        //     nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_OPEN, p->line_thickness);
        // } break;
        // case NK_COMMAND_TEXT: {
        //     const struct nk_command_text *t = (const struct nk_command_text*)cmd;
        //     nk_draw_list_add_text(&ctx->draw_list, t->font, nk_rect(t->x, t->y, t->w, t->h),
        //         t->string, t->length, t->height, t->foreground);
        // } break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *i = (const struct nk_command_image*)cmd;
            
            struct fb_image im;
            im.dx = i->x;
            im.dy = i->y;
            im.width = i->w;
            im.height = i->h;
            im.bg_color = nk_color_converter(i->col);
            im.fg_color = nk_color_converter(i->col);
            im.data = i->img.handle.ptr;//const char *data;
            im.depth = 1;


            //void (*fb_imageblit)(struct fb_info *info, const struct fb_image *image);
            fb_imageblit(vc->fb, &im);
            //nk_draw_list_add_image(&ctx->draw_list, i->img, nk_rect(i->x, i->y, i->w, i->h), i->col);
        } break;
        default: break;
        }
    }
    nk_clear(ctx);

    /* delay for remove flickering */
    for (int i = 0; i<100000000; i++){}
}


static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
    int x, int y, int width, int height, struct nk_color background_color){
    /* save style properties which will be overwritten */
    canvas->panel_padding = ctx->style.window.padding;
    canvas->item_spacing = ctx->style.window.spacing;
    canvas->window_background = ctx->style.window.fixed_background;
    
    /* use the complete window space and set background */
    ctx->style.window.spacing = nk_vec2(0,0);
    ctx->style.window.padding = nk_vec2(0,0);
    ctx->style.window.fixed_background = nk_style_item_color(background_color);
    
    /* create/update window and set position + size */
    flags = flags & ~NK_WINDOW_DYNAMIC;
    //nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);
    nk_begin(ctx, "Demo", nk_rect(50, 50, 300, 400),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_TITLE);
    //nk_window_set_bounds(ctx, nk_rect(x, y, width, height));
   
    /* allocate the complete window space for drawing */
    {struct nk_rect total_space;
    total_space = nk_window_get_content_region(ctx);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&total_space, ctx);
    canvas->painter = nk_window_get_canvas(ctx);}
    //canvas.painter->use_clipping = NK_CLIPPING_OFF;  
     
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas){
    nk_end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}

/* callbacks */
static void inpevent(struct vc *vc, struct input_event *ev){
}

static void visd(struct vc *vc, struct fb_info *fbinfo){

    /* fill all window with white */
    struct fb_fillrect rect;
    rect.dx = 0;
    rect.dy = 0;
    rect.width = 1024;
    rect.height = 1024;
    rect.rop = ROP_COPY;
    rect.color = 0xffff;

    fb_fillrect(vc->fb, &rect);
}

static void devisn(struct vc *vc) {
	mpx_devisualized(vc);
}

const struct vc_callbacks thiscbs = {
	.handle_input_event = inpevent,
	.visualized = visd,
	.schedule_devisualization = devisn,
};


int main(int argc, char *argv[]) {

	/* register callbacks */
	struct vc this_vc = {
		.name = "simple app",
		.callbacks = &thiscbs,
	};

	mpx_register_vc(&this_vc);


    /* GUI */
    struct device device;
    struct nk_font_atlas atlas;
    struct nk_font *font;
    struct nk_context ctx;

    int width = 0, 
        height = 0;

    
    nk_buffer_init_default(&device.cmds);
     
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    font = nk_font_atlas_add_default(&atlas, 13, 0);
    nk_font_atlas_bake(&atlas, &width, &height, NK_FONT_ATLAS_RGBA32);
    nk_font_atlas_end(&atlas, nk_handle_id((int)device.font_tex)/*nk_handle_id(0)*/, &device.null);

    nk_init_default(&ctx, &font->handle);

    struct media media;
    int x,y,n;
    media.skin = *stbi_load("../gwen.png", &x, &y, &n, 0);

    //int skin = image_load("../gwen.png");
    // struct nk_image window = nk_subimage_id(1, 512,512, nk_rect(128,23,127,104));
    // ctx.style.window.background = nk_rgb(204,204,204);
    // ctx.style.window.fixed_background = nk_style_item_image(window);
    // ctx.style.window.border_color = nk_rgb(67,67,67);
    // ctx.style.window.combo_border_color = nk_rgb(67,67,67);
    // ctx.style.window.contextual_border_color = nk_rgb(67,67,67);
    // ctx.style.window.menu_border_color = nk_rgb(67,67,67);
    // ctx.style.window.group_border_color = nk_rgb(67,67,67);
    // ctx.style.window.tooltip_border_color = nk_rgb(67,67,67);
    // ctx.style.window.scrollbar_size = nk_vec2(16,16);
    // ctx.style.window.border_color = nk_rgba(0,0,0,0);
    // ctx.style.window.padding = nk_vec2(8,4);
    // ctx.style.window.border = 3;

     {   /* skin */
        media.skin = 1;//image_load("../skins/gwen.png");
        media.check = nk_subimage_id(media.skin, 512,512, nk_rect(464,32,15,15));
        media.check_cursor = nk_subimage_id(media.skin, 512,512, nk_rect(450,34,11,11));
        media.option = nk_subimage_id(media.skin, 512,512, nk_rect(464,64,15,15));
        media.option_cursor = nk_subimage_id(media.skin, 512,512, nk_rect(451,67,9,9));
        media.header = nk_subimage_id(media.skin, 512,512, nk_rect(128,0,127,24));
        media.window = nk_subimage_id(media.skin, 512,512, nk_rect(128,23,127,104));
        media.scrollbar_inc_button = nk_subimage_id(media.skin, 512,512, nk_rect(464,256,15,15));
        media.scrollbar_inc_button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(464,320,15,15));
        media.scrollbar_dec_button = nk_subimage_id(media.skin, 512,512, nk_rect(464,224,15,15));
        media.scrollbar_dec_button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(464,288,15,15));
        media.button = nk_subimage_id(media.skin, 512,512, nk_rect(384,336,127,31));
        media.button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(384,368,127,31));
        media.button_active = nk_subimage_id(media.skin, 512,512, nk_rect(384,400,127,31));
        media.tab_minimize = nk_subimage_id(media.skin, 512,512, nk_rect(451, 99, 9, 9));
        media.tab_maximize = nk_subimage_id(media.skin, 512,512, nk_rect(467,99,9,9));
        media.slider = nk_subimage_id(media.skin, 512,512, nk_rect(418,33,11,14));
        media.slider_hover = nk_subimage_id(media.skin, 512,512, nk_rect(418,49,11,14));
        media.slider_active = nk_subimage_id(media.skin, 512,512, nk_rect(418,64,11,14));

        /* window */
        ctx.style.window.background = nk_rgb(204,204,204);
        ctx.style.window.fixed_background = nk_style_item_image(media.window);
        ctx.style.window.border_color = nk_rgb(67,67,67);
        ctx.style.window.combo_border_color = nk_rgb(67,67,67);
        ctx.style.window.contextual_border_color = nk_rgb(67,67,67);
        ctx.style.window.menu_border_color = nk_rgb(67,67,67);
        ctx.style.window.group_border_color = nk_rgb(67,67,67);
        ctx.style.window.tooltip_border_color = nk_rgb(67,67,67);
        ctx.style.window.scrollbar_size = nk_vec2(16,16);
        ctx.style.window.border_color = nk_rgba(0,0,0,0);
        ctx.style.window.padding = nk_vec2(8,4);
        ctx.style.window.border = 3;

        /* window header */
        //ctx.style.window.header;
        ctx.style.window.header.normal = nk_style_item_image(media.header);
        ctx.style.window.header.hover = nk_style_item_image(media.header);
        ctx.style.window.header.active = nk_style_item_image(media.header);
        ctx.style.window.header.label_normal = nk_rgb(95,95,95);
        ctx.style.window.header.label_hover = nk_rgb(95,95,95);
        ctx.style.window.header.label_active = nk_rgb(95,95,95);

        /* scrollbar */
        ctx.style.scrollv.normal          = nk_style_item_color(nk_rgb(184,184,184));
        ctx.style.scrollv.hover           = nk_style_item_color(nk_rgb(184,184,184));
        ctx.style.scrollv.active          = nk_style_item_color(nk_rgb(184,184,184));
        ctx.style.scrollv.cursor_normal   = nk_style_item_color(nk_rgb(220,220,220));
        ctx.style.scrollv.cursor_hover    = nk_style_item_color(nk_rgb(235,235,235));
        ctx.style.scrollv.cursor_active   = nk_style_item_color(nk_rgb(99,202,255));
        ctx.style.scrollv.dec_symbol      = NK_SYMBOL_NONE;
        ctx.style.scrollv.inc_symbol      = NK_SYMBOL_NONE;
        ctx.style.scrollv.show_buttons    = nk_true;
        ctx.style.scrollv.border_color    = nk_rgb(81,81,81);
        ctx.style.scrollv.cursor_border_color = nk_rgb(81,81,81);
        ctx.style.scrollv.border          = 1;
        ctx.style.scrollv.rounding        = 0;
        ctx.style.scrollv.border_cursor   = 1;
        ctx.style.scrollv.rounding_cursor = 2;

        /* scrollbar buttons */
        ctx.style.scrollv.inc_button.normal          = nk_style_item_image(media.scrollbar_inc_button);
        ctx.style.scrollv.inc_button.hover           = nk_style_item_image(media.scrollbar_inc_button_hover);
        ctx.style.scrollv.inc_button.active          = nk_style_item_image(media.scrollbar_inc_button_hover);
        ctx.style.scrollv.inc_button.border_color    = nk_rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_normal     = nk_rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_hover      = nk_rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_active     = nk_rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.border          = 0.0f;

        ctx.style.scrollv.dec_button.normal          = nk_style_item_image(media.scrollbar_dec_button);
        ctx.style.scrollv.dec_button.hover           = nk_style_item_image(media.scrollbar_dec_button_hover);
        ctx.style.scrollv.dec_button.active          = nk_style_item_image(media.scrollbar_dec_button_hover);
        ctx.style.scrollv.dec_button.border_color    = nk_rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_normal     = nk_rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_hover      = nk_rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_active     = nk_rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.border          = 0.0f;

        /* checkbox toggle */
        {struct nk_style_toggle *toggle;
        toggle = &ctx.style.checkbox;
        toggle->normal          = nk_style_item_image(media.check);
        toggle->hover           = nk_style_item_image(media.check);
        toggle->active          = nk_style_item_image(media.check);
        toggle->cursor_normal   = nk_style_item_image(media.check_cursor);
        toggle->cursor_hover    = nk_style_item_image(media.check_cursor);
        toggle->text_normal     = nk_rgb(95,95,95);
        toggle->text_hover      = nk_rgb(95,95,95);
        toggle->text_active     = nk_rgb(95,95,95);}

        /* option toggle */
        {struct nk_style_toggle *toggle;
        toggle = &ctx.style.option;
        toggle->normal          = nk_style_item_image(media.option);
        toggle->hover           = nk_style_item_image(media.option);
        toggle->active          = nk_style_item_image(media.option);
        toggle->cursor_normal   = nk_style_item_image(media.option_cursor);
        toggle->cursor_hover    = nk_style_item_image(media.option_cursor);
        toggle->text_normal     = nk_rgb(95,95,95);
        toggle->text_hover      = nk_rgb(95,95,95);
        toggle->text_active     = nk_rgb(95,95,95);}

        /* default button */
        ctx.style.button.normal = nk_style_item_image(media.button);
        ctx.style.button.hover = nk_style_item_image(media.button_hover);
        ctx.style.button.active = nk_style_item_image(media.button_active);
        ctx.style.button.border_color = nk_rgba(0,0,0,0);
        ctx.style.button.text_background = nk_rgba(0,0,0,0);
        ctx.style.button.text_normal = nk_rgb(95,95,95);
        ctx.style.button.text_hover = nk_rgb(95,95,95);
        ctx.style.button.text_active = nk_rgb(95,95,95);

        /* default text */
        ctx.style.text.color = nk_rgb(95,95,95);

        /* contextual button */
        ctx.style.contextual_button.normal = nk_style_item_color(nk_rgb(206,206,206));
        ctx.style.contextual_button.hover = nk_style_item_color(nk_rgb(229,229,229));
        ctx.style.contextual_button.active = nk_style_item_color(nk_rgb(99,202,255));
        ctx.style.contextual_button.border_color = nk_rgba(0,0,0,0);
        ctx.style.contextual_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.contextual_button.text_normal = nk_rgb(95,95,95);
        ctx.style.contextual_button.text_hover = nk_rgb(95,95,95);
        ctx.style.contextual_button.text_active = nk_rgb(95,95,95);

        /* menu button */
        ctx.style.menu_button.normal = nk_style_item_color(nk_rgb(206,206,206));
        ctx.style.menu_button.hover = nk_style_item_color(nk_rgb(229,229,229));
        ctx.style.menu_button.active = nk_style_item_color(nk_rgb(99,202,255));
        ctx.style.menu_button.border_color = nk_rgba(0,0,0,0);
        ctx.style.menu_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.menu_button.text_normal = nk_rgb(95,95,95);
        ctx.style.menu_button.text_hover = nk_rgb(95,95,95);
        ctx.style.menu_button.text_active = nk_rgb(95,95,95);

        /* tree */
        ctx.style.tab.text = nk_rgb(95,95,95);
        ctx.style.tab.tab_minimize_button.normal = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.hover = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.active = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_normal = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_hover = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_active = nk_rgba(0,0,0,0);

        ctx.style.tab.tab_maximize_button.normal = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.hover = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.active = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_normal = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_hover = nk_rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_active = nk_rgba(0,0,0,0);

        ctx.style.tab.node_minimize_button.normal = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.hover = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.active = nk_style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_normal = nk_rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_hover = nk_rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_active = nk_rgba(0,0,0,0);

        ctx.style.tab.node_maximize_button.normal = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.hover = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.active = nk_style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_normal = nk_rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_hover = nk_rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_active = nk_rgba(0,0,0,0);

        /* selectable */
        ctx.style.selectable.normal = nk_style_item_color(nk_rgb(206,206,206));
        ctx.style.selectable.hover = nk_style_item_color(nk_rgb(206,206,206));
        ctx.style.selectable.pressed = nk_style_item_color(nk_rgb(206,206,206));
        ctx.style.selectable.normal_active = nk_style_item_color(nk_rgb(185,205,248));
        ctx.style.selectable.hover_active = nk_style_item_color(nk_rgb(185,205,248));
        ctx.style.selectable.pressed_active = nk_style_item_color(nk_rgb(185,205,248));
        ctx.style.selectable.text_normal = nk_rgb(95,95,95);
        ctx.style.selectable.text_hover = nk_rgb(95,95,95);
        ctx.style.selectable.text_pressed = nk_rgb(95,95,95);
        ctx.style.selectable.text_normal_active = nk_rgb(95,95,95);
        ctx.style.selectable.text_hover_active = nk_rgb(95,95,95);
        ctx.style.selectable.text_pressed_active = nk_rgb(95,95,95);

        /* slider */
        ctx.style.slider.normal          = nk_style_item_hide();
        ctx.style.slider.hover           = nk_style_item_hide();
        ctx.style.slider.active          = nk_style_item_hide();
        ctx.style.slider.bar_normal      = nk_rgb(156,156,156);
        ctx.style.slider.bar_hover       = nk_rgb(156,156,156);
        ctx.style.slider.bar_active      = nk_rgb(156,156,156);
        ctx.style.slider.bar_filled      = nk_rgb(156,156,156);
        ctx.style.slider.cursor_normal   = nk_style_item_image(media.slider);
        ctx.style.slider.cursor_hover    = nk_style_item_image(media.slider_hover);
        ctx.style.slider.cursor_active   = nk_style_item_image(media.slider_active);
        ctx.style.slider.cursor_size     = nk_vec2(16.5f,21);
        ctx.style.slider.bar_height      = 1;

        /* progressbar */
        ctx.style.progress.normal = nk_style_item_color(nk_rgb(231,231,231));
        ctx.style.progress.hover = nk_style_item_color(nk_rgb(231,231,231));
        ctx.style.progress.active = nk_style_item_color(nk_rgb(231,231,231));
        ctx.style.progress.cursor_normal = nk_style_item_color(nk_rgb(63,242,93));
        ctx.style.progress.cursor_hover = nk_style_item_color(nk_rgb(63,242,93));
        ctx.style.progress.cursor_active = nk_style_item_color(nk_rgb(63,242,93));
        ctx.style.progress.border_color = nk_rgb(114,116,115);
        ctx.style.progress.padding = nk_vec2(0,0);
        ctx.style.progress.border = 2;
        ctx.style.progress.rounding = 1;

        /* combo */
        ctx.style.combo.normal = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.hover = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.active = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.border_color = nk_rgb(95,95,95);
        ctx.style.combo.label_normal = nk_rgb(95,95,95);
        ctx.style.combo.label_hover = nk_rgb(95,95,95);
        ctx.style.combo.label_active = nk_rgb(95,95,95);
        ctx.style.combo.border = 1;
        ctx.style.combo.rounding = 1;

        /* combo button */
        ctx.style.combo.button.normal = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.button.hover = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.button.active = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.combo.button.text_background = nk_rgb(216,216,216);
        ctx.style.combo.button.text_normal = nk_rgb(95,95,95);
        ctx.style.combo.button.text_hover = nk_rgb(95,95,95);
        ctx.style.combo.button.text_active = nk_rgb(95,95,95);

        /* property */
        ctx.style.property.normal = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.hover = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.active = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.border_color = nk_rgb(81,81,81);
        ctx.style.property.label_normal = nk_rgb(95,95,95);
        ctx.style.property.label_hover = nk_rgb(95,95,95);
        ctx.style.property.label_active = nk_rgb(95,95,95);
        ctx.style.property.sym_left = NK_SYMBOL_TRIANGLE_LEFT;
        ctx.style.property.sym_right = NK_SYMBOL_TRIANGLE_RIGHT;
        ctx.style.property.rounding = 10;
        ctx.style.property.border = 1;

        /* edit */
        ctx.style.edit.normal = nk_style_item_color(nk_rgb(240,240,240));
        ctx.style.edit.hover = nk_style_item_color(nk_rgb(240,240,240));
        ctx.style.edit.active = nk_style_item_color(nk_rgb(240,240,240));
        ctx.style.edit.border_color = nk_rgb(62,62,62);
        ctx.style.edit.cursor_normal = nk_rgb(99,202,255);
        ctx.style.edit.cursor_hover = nk_rgb(99,202,255);
        ctx.style.edit.cursor_text_normal = nk_rgb(95,95,95);
        ctx.style.edit.cursor_text_hover = nk_rgb(95,95,95);
        ctx.style.edit.text_normal = nk_rgb(95,95,95);
        ctx.style.edit.text_hover = nk_rgb(95,95,95);
        ctx.style.edit.text_active = nk_rgb(95,95,95);
        ctx.style.edit.selected_normal = nk_rgb(99,202,255);
        ctx.style.edit.selected_hover = nk_rgb(99,202,255);
        ctx.style.edit.selected_text_normal = nk_rgb(95,95,95);
        ctx.style.edit.selected_text_hover = nk_rgb(95,95,95);
        ctx.style.edit.border = 1;
        ctx.style.edit.rounding = 2;

        /* property buttons */
        ctx.style.property.dec_button.normal = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.dec_button.hover = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.dec_button.active = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.dec_button.text_background = nk_rgba(0,0,0,0);
        ctx.style.property.dec_button.text_normal = nk_rgb(95,95,95);
        ctx.style.property.dec_button.text_hover = nk_rgb(95,95,95);
        ctx.style.property.dec_button.text_active = nk_rgb(95,95,95);
        ctx.style.property.inc_button = ctx.style.property.dec_button;

        /* property edit */
        ctx.style.property.edit.normal = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.edit.hover = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.edit.active = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.property.edit.border_color = nk_rgba(0,0,0,0);
        ctx.style.property.edit.cursor_normal = nk_rgb(95,95,95);
        ctx.style.property.edit.cursor_hover = nk_rgb(95,95,95);
        ctx.style.property.edit.cursor_text_normal = nk_rgb(216,216,216);
        ctx.style.property.edit.cursor_text_hover = nk_rgb(216,216,216);
        ctx.style.property.edit.text_normal = nk_rgb(95,95,95);
        ctx.style.property.edit.text_hover = nk_rgb(95,95,95);
        ctx.style.property.edit.text_active = nk_rgb(95,95,95);
        ctx.style.property.edit.selected_normal = nk_rgb(95,95,95);
        ctx.style.property.edit.selected_hover = nk_rgb(95,95,95);
        ctx.style.property.edit.selected_text_normal = nk_rgb(216,216,216);
        ctx.style.property.edit.selected_text_hover = nk_rgb(216,216,216);

        /* chart */
        ctx.style.chart.background = nk_style_item_color(nk_rgb(216,216,216));
        ctx.style.chart.border_color = nk_rgb(81,81,81);
        ctx.style.chart.color = nk_rgb(95,95,95);
        ctx.style.chart.selected_color = nk_rgb(255,0,0);
        ctx.style.chart.border = 1;
    }
    



    /* Draw */
    struct nk_canvas canvas;
    while (1) 
    {
        /* what to draw */
        canvas_begin(&ctx, &canvas, 0, 0, 0, width, height, nk_rgb(100,100,100));
        {

            

        }
        canvas_end(&ctx, &canvas);

         /* Draw each element */
         device_draw(&this_vc, &device, &ctx, width, height);
     }
    //stbi_image_free(media.skin);
    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    nk_buffer_free(&device.cmds);

    printf("\nEnd of program.\nIf you see it then something goes wrong.\n");
    return 0;

}
