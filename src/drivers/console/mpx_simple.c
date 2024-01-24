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
#include <drivers/input/input_dev.h>
#include <lib/libds/dlist.h>
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
	vc->fb = NULL;

}

static int mpx_visualize(struct vc *vc) {

	if (postvc) {
		return -EAGAIN;
	}

	postvc = vc;

	if (curvc) {
		curvc->callbacks->schedule_devisualization(curvc);
	} else {
		mpx_devisualized(curvc);
	}

	return 0;

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
	if (!(cbs && cbs->handle_input_event
			&& cbs->visualized
			&& cbs->schedule_devisualization)) {
		return -EINVAL;
	}

	if (curvc == NULL) {
		mpx_visualize(vc);
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

int mpx_devisualized(struct vc *vc) {

	if (curvc != vc) {
		return -EINVAL;
	}

	if (curvc) {
		curvc->fb = NULL;
	}

	postvc->fb = curfb;
	postvc->callbacks->visualized(postvc, curfb);

	curvc = postvc;
	postvc = NULL;

	return 0;
}

static int indev_event_cb(struct input_dev *indev) {
	struct input_event ev;
	const int f1 = 0x3b;

	while (0 <= input_dev_event(indev, &ev)) {

		if (ev.type == KBD_KEY_PRESSED && (ev.value & KBD_CTRL_PRESSED)) {
			int num = (ev.value & 0x7f) - f1;
			if (num >= 0 && num < VC_MPX_N && vcs[num] != NULL) {
				mpx_visualize(vcs[num]);
				return 0;
			}
		}

		if (curvc) {
			curvc->callbacks->handle_input_event(curvc, &ev);
		}
	}

	return 0;
}

static int vc_mpx_init(void) {
	struct input_dev *indev;
	const char *devlist[] = {"ps-keyboard", "ps-mouse", NULL};
	const char **p;

	for (p = devlist; *p != NULL; p++) {
		if (!(indev = input_dev_lookup(*p))) {
			continue;
		}

		if (0 > input_dev_open(indev, indev_event_cb)) {
			continue;
		}
	}

	if (NULL == (curfb = fb_lookup(0))) {
		return -ENOENT;
	}
	return 0;
}

