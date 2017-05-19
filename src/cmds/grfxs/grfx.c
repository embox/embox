/* This file provides simple example of
work nuklear on OS Embox */

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

/* callbacks */
static void inpevent(struct vc *vc, struct input_event *ev)
{
    //printf("\nWhat are doing with graphic window??	\n");
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

struct vc this_vc = {
		.name = "simple app",
		.callbacks = &thiscbs,
};

float your_text_width_calculation(nk_handle handle, float height, const char *text, int len){
    return 16;
}

int main(int argc, char *argv[]) {
/* register callbacks */
		mpx_register_vc(&this_vc);
  
    /* GUI */
    static struct nk_context ctx;
    
    int width = 0, 
        height = 0;

    static struct nk_user_font font;
    font.userdata.ptr = &width;
    font.height = font_vga_8x16.height;
    font.width = your_text_width_calculation;
    nk_init_default(&ctx, &font);
    
    width = this_vc.fb->var.xres;
    height = this_vc.fb->var.yres;
    

	while(1) {
		draw(&this_vc, &ctx, width, height);
	}

	/* end of work */
    nk_free(&ctx);
    
	/* checking working command */
	printf("\nJob's done! :)\n\n");

	return 0;
}
