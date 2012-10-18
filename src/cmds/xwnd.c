/**
 * @file
 * @brief Here is the entry point for XWnd.
 *
 * @date Oct 1, 2012
 * @author Alexandr Chernakov
 */


#include <stdio.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>
#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>
#include <xwnd/app_registry.h>

EMBOX_CMD(exec);


int xwnd_init(void);
int xwnd_start(void);
int xwnd_widgets_init(void);
void xwnd_quit(void);

int xwnd_send_app_msg(void);

int xwnd_init() {
	vesa_init_mode(VESA_MODE_DEFAULT);
	vesa_clear_screen();
	return 0;
}
void xwnd_quit(){
	vesa_quit_mode();
}

static int exec (int argc, char ** argv) {
	struct xwnd_bmp_image * img;

	if (argc > 1) {
		img = xwnd_bmp_load(argv[1]);
		if (!img) {
			printf ("Failed to load bmp, error code %d\n", xwnd_bmp_get_errno());
			return 1;
		}

		if (argc == 2) {
			xwnd_init();

			xwnd_bmp_draw(img);
			xwnd_bmp_unload(img);
			while (1) {
				if (!keyboard_has_symbol()) {
					usleep(100);
				}
				else if ('q' == keyboard_getc()) {
					break;
				}
			}

			xwnd_quit();
			return 0;
		}
		if (argc > 2)
		{

			printf("w/h: %d/%d, bpp: %d, ERR: %d\n", img->width, img->height, img->bpp, xwnd_bmp_get_errno());
			xwnd_bmp_unload(img);
			return 0;
		}
	}
	else {
		int err;
		err = xwnd_app_reg_init();
		if (err) {
			printf ("AAAAA");
			return 1;
		}
		return 0;
	}
	return 1;
}
