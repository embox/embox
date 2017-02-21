#include <stdio.h>

/* includes for nuklear*/
#define NK_PRIVATE
#define NK_API
#define NK_INTERN
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
#include <drivers/console/fbcon.h>
#include <drivers/video/fb.h>
#include <drivers/input/input_dev.h>

/* callbacks */
static void inpevent(struct vc *vc, struct input_event *ev){
    printf("\nWhat are doing with graphic window??	\n");
}

static void visd(struct vc *vc, struct fb_info *fbinfo)
{
    struct fb_var_screeninfo var;
    struct fb_info *fb = vc->fb;

    if (0 != fb_get_var(fbinfo, &var)) {
	 	return;
	}

    /* portable */
    struct fb_fillrect rect;
    rect.dx = 200;
    rect.dy = 200;
    rect.width = 200;
    rect.height = 200;
    rect.color = 0xf00;
    rect.rop = ROP_COPY;
    fb_fillrect(vc->fb, &rect);

    /* VERY not portable */
    for (int i = 0; i < 200; ++i) {
	    for (int j = 0; j < 200; ++j) {
		    *(((uint16_t*)fb->screen_base) + i * var.xres + j) = ((i * 32 / 200) << 11) + (0x1f & (j * 32 / 200));
	    }
    }
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

	/* initial items for nuklear */
    struct nk_font_atlas atlas;
	struct nk_font *font;
	struct nk_context ctx;
	
	
	nk_font_atlas_init_default(&atlas);
	nk_font_atlas_begin(&atlas);

	font = nk_font_atlas_add_default(&atlas, 13, 0);
	nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	nk_font_atlas_end(&atlas, nk_handle_id(0), 0);
	
	nk_init_default(&ctx, &font->handle);
	
	/* start of work with nuklear buffers */
	

	while(1) {}

	/* end of work */
    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
        

    /* checking working command */
	printf("\nJob's done! :)\n\n");

	return 0;
}
