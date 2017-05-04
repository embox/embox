/* This file provides an example of work nuklear on OS Embox */

/* includes for nuklear*/
#include <stdio.h>
#define NK_PRIVATE
#define NK_API
#define NK_INTERN static
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "nk_embox_render.h"


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
    nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);
    //nk_window_set_bounds(ctx, nk_rect(x, y, width, height));
   
    /* allocate the complete window space for drawing */
    {struct nk_rect total_space;
    total_space = nk_window_get_content_region(ctx);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&total_space, ctx);
    canvas->painter = nk_window_get_canvas(ctx);}
     
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
    struct nk_canvas canvas;

    int width = 0, 
        height = 0;

    
    nk_buffer_init_default(&device.cmds);
 
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    font = nk_font_atlas_add_default(&atlas, 13, 0);
    nk_font_atlas_bake(&atlas, &width, &height, NK_FONT_ATLAS_RGBA32);
    nk_font_atlas_end(&atlas, nk_handle_id(0), &device.null);

    nk_init_default(&ctx, &font->handle);

    /* Draw */
    while (1) 
    {
        /* what to draw */
        canvas_begin(&ctx, &canvas, 0, 0, 0, width, height, nk_rgb(100,100,100));
        {

            canvas.painter->use_clipping = NK_CLIPPING_OFF;  

            nk_fill_rect(canvas.painter, nk_rect(15,15,210,210), 5, nk_rgb(247, 230, 154));
            nk_fill_rect(canvas.painter, nk_rect(20,20,200,200), 5, nk_rgb(188, 174, 118));
            nk_draw_text(canvas.painter, nk_rect(30, 30, 150, 20), "Text to draw", 12, &font->handle, nk_rgb(188,174,118), nk_rgb(0,0,0));
            nk_fill_rect(canvas.painter, nk_rect(250,20,100,100), 0, nk_rgb(0,0,255));
            nk_fill_circle(canvas.painter, nk_rect(20,250,100,100), nk_rgb(255,0,0));
            nk_fill_triangle(canvas.painter, 250, 250, 350, 250, 300, 350, nk_rgb(0,255,0));
            nk_fill_arc(canvas.painter, 300, 180, 50, 0, 3.141592654f * 3.0f / 4.0f, nk_rgb(255,255,0));

            {float points[12];
            points[0] = 200; points[1] = 250;
            points[2] = 250; points[3] = 350;
            points[4] = 225; points[5] = 350;
            points[6] = 200; points[7] = 300;
            points[8] = 175; points[9] = 350;
            points[10] = 150; points[11] = 350;
            nk_fill_polygon(canvas.painter, points, 6, nk_rgb(0,0,0));}

            nk_stroke_line(canvas.painter, 15, 10, 200, 10, 2.0f, nk_rgb(189,45,75));
            nk_stroke_rect(canvas.painter, nk_rect(370, 20, 100, 100), 10, 3, nk_rgb(0,0,255));
            nk_stroke_curve(canvas.painter, 380, 200, 405, 270, 455, 120, 480, 200, 2, nk_rgb(0,150,220));
            nk_stroke_circle(canvas.painter, nk_rect(20, 370, 100, 100), 5, nk_rgb(0,255,120));
            nk_stroke_triangle(canvas.painter, 370, 250, 470, 250, 420, 350, 6, nk_rgb(255,0,143));

            /* load some image */
            int x,y,n;
            unsigned char * data = stbi_load("201.png", &x, &y, &n, 0);
            if (data == NULL)
                printf("\nstbi_load doesn't work. :(\n");

            struct nk_image im;
            im.handle.ptr = data;
            im.handle.id = (uint32_t)data;
            im.w = x;
            im.h = y;
            im.region[0] = 0;
            im.region[1] = 0;
            im.region[2] = x;
            im.region[3] = y;
            
            nk_draw_image(canvas.painter, nk_rect(0, 0, 999, 799), &im, nk_rgb(100, 0, 0));

            stbi_image_free(data);            
        }
        canvas_end(&ctx, &canvas);

         /* Draw each element */
         draw(&this_vc, &ctx, width, height);
    }

    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    nk_buffer_free(&device.cmds);

    printf("\nEnd of program.\nIf you see it then something goes wrong.\n");
    return 0;
}
