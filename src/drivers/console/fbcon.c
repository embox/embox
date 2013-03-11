/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.03.2013
 */

#include <errno.h>
#include <string.h>
#include <drivers/console/mpx.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(fbcon_init);

static void handle_input_event(struct vc *vc, struct input_event *ev);
static void visualized(struct vc *vc, struct fb_info *fbinfo);
static void schedule_devisualization(struct vc *vc, vc_devizcb_t cb);

const struct vc_callbacks thiscbs = {
	.handle_input_event = handle_input_event,
	.visualized = visualized,
	.schedule_devisualization = schedule_devisualization,
};

struct vc thisvc = {
	.name = "fbcon",
	.callbacks = &thiscbs,
};

struct vc thisvc2 = {
	.name = "fbcon2",
	.callbacks = &thiscbs,
};

static void handle_input_event(struct vc *vc, struct input_event *ev) {

}

static void visualized(struct vc *vc, struct fb_info *fbinfo) {
	struct fb_fillrect fbrect = {
		.dx = 0,
		.dy = 0,
		.width = fbinfo->var.xres,
		.height = fbinfo->var.yres,
		.color = 0,
		.rop = 0,
	};

	if (vc == &thisvc2) {
		fbrect.color = 0xff;
	}

	fb_fillrect(fbinfo, &fbrect);

}

static void schedule_devisualization(struct vc *vc, vc_devizcb_t cb) {
	cb(vc);
}

static int fbcon_init(void) {
	int ret;

	if (0 > (ret = mpx_register_vc(&thisvc))) {
		return ret;
	}

	return mpx_register_vc(&thisvc2);
}
