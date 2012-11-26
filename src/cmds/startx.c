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
#include <fcntl.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>

#include <xwnd/xappreg.h>

static int exec(int argc, char ** argv);

EMBOX_CMD(exec);


int xwnd_init(void) {
	vesa_init_mode(VESA_MODE_DEFAULT);
	vesa_clear_screen();
	return 0;
}

void xwnd_quit(void){
	vesa_quit_mode();
}

static int exec (int argc, char ** argv) {
	int err, i;
	struct xwnd_application_args args;
	args = args;

	if (argc < 2) {
		for (i = 0; i < ARRAY_SPREAD_SIZE(__xwnd_app_repository); i++) {
			printf("%s\n", __xwnd_app_repository[i].app_name);
		}
		return 0;
	} else {
		xwnd_init();


		if (argc < 3) {
			err = xwnd_app_start((const char *)argv[1], NULL);
		} else {
			err = xwnd_app_start((const char *)argv[1], argv[2]);
		}
		if (err) {
			xwnd_quit();
			return 1;
		}

		while (1) {
			if (!keyboard_has_symbol()) {
				usleep(100);
			}
			else {
				char key = keyboard_getc();
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
	}

	return 0;
}
