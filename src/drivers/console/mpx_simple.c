/**
 * @file
 * @brief
 *
 * @author  Felix Sulima
 * @author  Anton Kozlov
 * @date    06.03.2013
 */

#include <errno.h>
#include <drivers/console/mpx.h>
#include <drivers/keyboard.h>
#include <util/dlist.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(vc_mpx_init);

struct vc *vcs[VC_MPX_N];

static struct vc *curvc;
static struct vc *postvc;
static struct fb_info *curfb;

static struct vc **vcs_find(struct vc **vcs, struct vc *vc) {
	int i;
	for (i = 0; i < VC_MPX_N; i++) {
		if (vcs[i] != vc) {
			continue;
		}

		return &vcs[i];

	}
	return NULL;
}

static void vc_init(struct vc *vc) {

}

int mpx_register_vc(struct vc *vc) {
	struct vc **pvc = vcs_find(vcs, NULL);
	const struct vc_callbacks *cbs;

	if (!pvc) {
		return -ENOMEM;
	}

	*pvc = vc;

	vc_init(vc);

	cbs = vc->callbacks;
	if (!(cbs->handle_input_event
			&& cbs->visualized
			&& cbs->schedule_devisualization)) {
		return -EINVAL;
	}

	if (curvc == NULL) {
		mpx_visualized(vc);
	}

	return 0;
}

int mpx_deregister_vc(struct vc *vc) {
	struct vc **pvc;

	if (vc == curvc) {
		return -EBUSY;
	}

	if (!(pvc = vcs_find(vcs, vc))) {
		return -ENOENT;
	}

	*pvc = NULL;

	return 0;
}

static void mpx_deviz_callback(struct vc *vc) {

	postvc->fb = curfb;
	postvc->callbacks->visualized(postvc, curfb);

	postvc = NULL;
	curvc = vc;
}


int mpx_visualized(struct vc *vc) {
	struct vc *tvc = curvc;
	curvc = NULL;

	if (postvc) {
		return -EAGAIN;
	}

	postvc = vc;

	if (tvc) {
		tvc->callbacks->schedule_devisualization(tvc, mpx_deviz_callback);
	} else {
		mpx_deviz_callback(tvc);
	}

	return 0;

}

static int indev_event_cb(struct input_dev *indev) {
	struct input_event ev;
	const int f1 = 0x3b;

	input_dev_event(indev, &ev);

	if (ev.type == KEY_PRESSED && (ev.value & CTRL_PRESSED)) {
		int num = (ev.value & 0x7f) - f1;
		if (num < VC_MPX_N && vcs[num] != NULL) {
			mpx_visualized(vcs[num]);
			return 0;
		}
	}

	if (curvc) {
		curvc->callbacks->handle_input_event(curvc, &ev);
	}

	return 0;
}

static int vc_mpx_init(void) {
	struct input_dev *indev;

	if (!(indev = input_dev_lookup("keyboard"))) {
		return -ENOENT;
	}

	if (0 > input_dev_open(indev, indev_event_cb)) {
		return -EAGAIN;
	}

	if (NULL == (curfb = fb_lookup("fb0"))) {
		return -ENOENT;
	}

	return 0;
}

