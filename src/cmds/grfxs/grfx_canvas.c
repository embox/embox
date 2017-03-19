
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

/* includes from fbcon */
#include <drivers/console/mpx.h>
#include <drivers/video/fb.h>

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

int nk_color_converter(struct nk_color color){
    int b = (color.b / 8)  & (0x1f);
    int g = ((color.g / 8)<<6) & (0x7e0);
    int r = ((color.r / 8)<<11) & 0xf800;
    return r + g + b;
}
static void
device_draw( struct vc *vc, struct nk_context *ctx, int width, int height)
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
        }
        default: break;
        }
    }
    nk_clear(ctx);

    /* delay for remove flickering */
    for (int i = 0; i<100000000; i++){}
}


static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
    int x, int y, int width, int height, struct nk_color background_color)
{
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
    nk_window_set_bounds(ctx, nk_rect(x, y, width, height));
    
    /* allocate the complete window space for drawing */
    {struct nk_rect total_space;
    total_space = nk_window_get_content_region(ctx);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&total_space, ctx);
    canvas->painter = nk_window_get_canvas(ctx);}
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas)
{
    nk_end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}

/* callbacks */
static void inpevent(struct vc *vc, struct input_event *ev)
{
   printf("\nWhat are doing with graphic window??	\n");
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

    /* Draw */
    struct nk_canvas canvas;
    while (1) 
    {
        /* what to draw */
        canvas_begin(&ctx, &canvas, 0, 0, 0, width, height, nk_rgb(100,100,100));
        {
            nk_fill_rect(canvas.painter, nk_rect(15,15,210,210), 5, nk_rgb(50, 0, 250));
            nk_fill_rect(canvas.painter, nk_rect(280,100,100,100), 5, nk_rgb(0, 174, 118));
            nk_fill_rect(canvas.painter, nk_rect(250,20,100,100), 0, nk_rgb(180,0,0));
        }
        canvas_end(&ctx, &canvas);

         /* Draw each element */
         device_draw(&this_vc, &ctx, width, height);
     }
    
    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    nk_buffer_free(&device.cmds);

    printf("\nEnd of program.\nIf you see it then something goes wrong.\n");
    return 0;

}
