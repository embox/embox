/**
 * @file
 * @brief Here is the entry point for XWnd.
 *
 * @date Oct 1, 2012
 * @author Alexandr Chernakov
 */


#include <embox/cmd.h>
#include <stdio.h>
#include <vesa.h>
#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>
#include <asm/io.h>

void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x + i, y, c);
	}
}

void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x, y + i, c);
	}
}

void xwnd_draw_window (struct xwnd_window * wnd) {
	xwnd_draw_vert_line  (wnd->x, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y, wnd->wd, 2);
	xwnd_draw_vert_line  (wnd->x + wnd->wd, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y + wnd->ht, wnd->wd, 2);
}

EMBOX_CMD(exec);

static int exec (int argc, char ** argv) {
	struct xwnd_bmp_image img;

	if (argc > 1) {
		img = xwnd_bmp_load(argv[1]);
		if (img.err != XWND_BMP_OK) {
			printf ("Failed to load bmp, error code %d\n", img.err);
			return 1;
		}

		if (argc == 2) {
			vesa_init_mode(VESA_MODE_DEFAULT);
			vesa_clear_screen();
			xwnd_bmp_draw(&img);
			xwnd_bmp_unload(&img);
			//vesa_quit_mode();
			return 0;
		}
		if (argc > 2)
		{
			printf("w/h: %d/%d, bpp: %d, ERR: %d\n", img.width, img.height, img.bpp, img.err);
			xwnd_bmp_unload(&img);
			return 0;
		}
	}
	else {
		printf("Need more arguments\n");
		return 1;
	}
	return 1;
}
