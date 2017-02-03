//include embox.cmd.grfxs.grfx


#include <stdio.h>


#define NK_INTERN static inline
#define NK_API extern

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

static void inpevent(struct vc *vc, struct input_event *ev)
{
    printf("\nI do not want do anything ._.\n");
}

static void visd(struct vc *vc, struct fb_info *fbinfo)
{
    struct fb_var_screeninfo var;

    if (0 != fb_get_var(fbinfo, &var)) {
	 	return;
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

	struct nk_font_atlas atlas;
	//const void *img;
	//int w, h;
	struct nk_font *font;
	struct nk_context ctx;
	
	
	nk_font_atlas_init_default(&atlas);
	nk_font_atlas_begin(&atlas);
	font = nk_font_atlas_add_from_file(&atlas, "Roboto-Black.ttf", 13, 0);
	//img = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	//nk_handle texture;
	//nk_font_atlas_end(&atlas, nk_handle_id(texture), 0);
	
	
    nk_init_default(&ctx, &font->handle);
    nk_font_atlas_clear(&atlas);
    
    
    


	// //Start of example
	
	// nk_init_fixed(&ctx, calloc(1, 1024), 1024, &NK_INCLUDE_DEFAULT_FONT);

	// enum {EASY, HARD};
	// int op = EASY;
	// float value = 0.6f;
	// //int i =  20;

	// if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
	// 	NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
	// 	/* fixed widget pixel width */
	// 	nk_layout_row_static(&ctx, 30, 80, 1);
	// 	if (nk_button_label(&ctx, "button")) {
	// 		/* event handling */
	// 	}

	// 	/* fixed widget window ratio width */
	// 	nk_layout_row_dynamic(&ctx, 30, 2);
	// 	if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
	// 	if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

	// 	/* custom widget pixel width */
	// 	nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
	// 	{
	// 		nk_layout_row_push(&ctx, 50);
	// 		nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
	// 		nk_layout_row_push(&ctx, 110);
	// 		nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
	// 	}
	// 	nk_layout_row_end(&ctx);
	// }
	// nk_end(&ctx);
	// // End of example






    printf("\nThere is Sasha's command! :)\n\n");
	return 0;
}

