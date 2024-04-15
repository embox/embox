/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 * @author Vita Loginova
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <termios.h>
#include <lib/libds/array.h>
#include <util/member.h>
#include <drivers/video_term.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/keymap.h>
#include <drivers/tty.h>

static const char *vterm_key_to_esc(int keycode){
	switch (keycode) {
	case KEY_INS: return "2~";
	case KEY_DEL: return "3~";
	case KEY_HOME: return "H";
	case KEY_END: return "F";
	case KEY_PGUP: return "5~";
	case KEY_PGDN: return "6~";
	case KEY_UP: return "A";
	case KEY_DOWN: return "B";
	case KEY_LEFT: return "D";
	case KEY_RGHT: return "C";
	}

	return NULL;
}

static char vterm_key_to_char(int keycode) {
	switch (keycode) {
	case 0:
	case KEY_CAPS:
	case KEY_SHFT:
	case KEY_CTRL:
	case KEY_ALT:
		return 0; /* no ascii symbols */

	case KEY_F1 ... KEY_F12:
		return 0; /* no ascii symbols */

	default:
		return (char) keycode;
	}
}

int vterm_input(struct vterm *vt, struct input_event *event) {
	const char *esc_body;
	int keycode;

	keycode = keymap_kbd(event);
	if (keycode < 0 || !(event->type & KBD_KEY_PRESSED)) {
		return 0;
	}

	esc_body = vterm_key_to_esc(keycode);

	if (esc_body) {
		tty_rx_putc(&vt->tty, 0x1B, 0);
		tty_rx_putc(&vt->tty, '[', 0);

		for (const char *pch = esc_body; *pch; ++pch)
			tty_rx_putc(&vt->tty, *pch, 0);

	} else {
		char ch = vterm_key_to_char(keycode);

		if (ch)
			tty_rx_putc(&vt->tty, ch, 0);
	}

	return 0;
}

static int vterm_indev_eventhnd(struct input_dev *indev) {
	struct input_event event;

	while (0 == input_dev_event(indev, &event)) {
		vterm_input(indev->data, &event);
	}
	return 0;
}

void vterm_open_indev(struct vterm *vt, const char *name) {
	struct input_dev *dev;

	dev = input_dev_lookup(name);
	if (dev) {
		dev->data = vt;
		vt->indev = dev;
		input_dev_open(dev, &vterm_indev_eventhnd);
	}
}

static void vterm_tty_out_wake(struct tty *t) {
	struct vterm *vt = member_cast_out(t, struct vterm, tty);
	int ich;

	while ((ich = tty_out_getc(t)) != -1)
		vterm_video_putc(vt->video, (char) ich);
}

const struct tty_ops vterm_tty_ops = {
	.out_wake = &vterm_tty_out_wake,
};

void vterm_init(struct vterm *vt, struct vterm_video *video,
		struct input_dev *indev) {
	assert(video && video->ops && video->ops->init);

	vt->video = video;
	vt->indev = indev;

	vt->video->ops->init(vt->video);

	tty_init(&vt->tty, &vterm_tty_ops);

}
