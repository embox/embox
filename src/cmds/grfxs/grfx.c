//include embox.cmd.grfxs.grfx


#include <stdio.h>

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

    printf("\nThere is Sasha's command! :)\n\n");
	return 0;
}

