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
#include <drivers/video/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>

#include <xwnd/xappreg.h>
#include <lib/xwnd/application.h>

#include <drivers/video/display.h>
#include <drivers/video/fb.h>

static int exec(int argc, char ** argv);

EMBOX_CMD(exec);

static struct display display;

struct display *display_get(void) {
	return &display;
}

int xwnd_init(void) {
#if 0
	if(NULL == vga_setup_mode(&display, 0x13)) {
		return -1;
	}

	display_clear_screen(&display);
#else
	const unsigned short color1 = 0xF0, color2 = 0x0F,
		 	 color3 = 0x70, color4 = 0x07;
	struct fb_info *info = fb_lookup("fb0");
	info->ops->fb_set_par(info);
	fb_memset(info->screen_base, color1, info->screen_size / 4);
	fb_memset((info->screen_base + info->screen_size / 4),
			color2, info->screen_size / 4);
	fb_memset((info->screen_base + info->screen_size / 2),
			color3, info->screen_size / 4);
	fb_memset((info->screen_base + 3 * info->screen_size / 4),
			color4, info->screen_size / 4);
#endif
	return 0;
}


void xwnd_quit(void){
	display_clear_screen(&display);

	vga_setup_mode(&display, 0x3);
}

static int exec (int argc, char ** argv) {
	int err, i;

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
				msleep(100);
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
