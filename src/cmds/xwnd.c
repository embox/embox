/**
 * @file
 * @brief Here is the entry point for XWnd.
 *
 * @date Oct 1, 2012
 * @author Alexandr Chernakov
 */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>
#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>
#include <xwnd/app_registry.h>
#include <xwnd/test_app.h>
#include <xwnd/unit_test.h>
#include <xwnd/event.h>

EMBOX_CMD(exec);


int xwnd_init(void);
int xwnd_start(void);
int xwnd_widgets_init(void);
void xwnd_quit(void);

int xwnd_send_app_msg(void);

int xwnd_init() {
	xwnd_unit_test_init(NULL);
	xwnd_unit_test_add_test("XWND UnitTest testing itself", xwnd_unit_test_self_test);
	xwnd_unit_test_run_all_sheduled_tests ();
	xwnd_unit_test_quit();

	vesa_init_mode(VESA_MODE_DEFAULT);
	vesa_clear_screen();
	return 0;
}

void xwnd_quit(){
	vesa_quit_mode();
}

static int exec (int argc, char ** argv) {
	int err, i;
	struct xwnd_app_registry * xapp_reg;
	if (argc < 2) {
		for (i = 0; i < ARRAY_SPREAD_SIZE(__xwnd_app_repository); i++) {
			printf("%s\n", __xwnd_app_repository[i].app_name);
		}
		return 0;
	} else {
		xwnd_init();

		xapp_reg = xwnd_app_reg();
		if (!xapp_reg) {
			return 1;
		}
	//	xwnd_app_create(xwnd_term_main);
	//	xwnd_app_create(test_app_main);
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
					//xwnd_app_send_quit_event(app_id, 0);
					xwnd_event_broadcast_quit_event(xwnd_app_reg()->event_sup, 0);
					sleep(1);
					break;
				} else if ('a' == key) {
					xwnd_event_move_focus(xwnd_app_reg()->event_sup);
				} else {
					xwnd_event_send_kbd_event(xwnd_app_reg()->event_sup, key);
					xwnd_event_send_sys_event(xwnd_app_reg()->event_sup, XWND_EV_DRAW);
				}
			}
		}
		xwnd_quit();
	}

	return 0;
}
