/**
 * @file
 * @brief Here is the entry point for XWnd.
 *
 * @date Oct 19, 2012
 * @author Alexandr Chernakov
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <drivers/video/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>

#include <xwnd/xappreg.h>
#include <lib/xwnd/application.h>

#include <drivers/video/display.h>

EMBOX_CMD(exec);

static struct display display;

struct display *display_get(void) {
	return &display;
}

int xwnd_init(void) {
	if (NULL == vga_setup_mode(&display, 0x13)) {
		return -1;
	}

	display_clear_screen(&display);
	return 0;
}

void xwnd_quit(void) {
	display_clear_screen(&display);

	vga_setup_mode(&display, 0x3);
}

static int exec(int argc, char **argv) {
	int err;
	size_t i;
	char key;

	if (argc < 2) {
		for (i = 0; i < ARRAY_SPREAD_SIZE(__xwnd_app_repository); i++) {
			printf("%s\n", __xwnd_app_repository[i].app_name);
		}
		return ENOERR;
	}

	if (-1 == xwnd_init()) {
		print("Can't setup VGA mode\n");
		return -1;
	}

	err = xwnd_app_start((const char *) argv[1], argc < 3 ? NULL : argv[2]);
	if (err) {
		xwnd_quit();
		return -1;
	}

	while (1) {
		if (!keyboard_has_symbol()) {
			msleep(100);
		} else {
			key = keyboard_getc();
			if ('q' == key) {
				xwnd_app_registry_broadcast_quit_event();
				sleep(1);
				break;
			} else if ('a' == key) {
				xwnd_app_registry_move_focus();
			} else {
				xwnd_app_registry_send_kbd_event(key);
				xwnd_app_registry_send_sys_event(XEV_DRAW);
			}
		}
	}
	xwnd_quit();

	return ENOERR;
}
