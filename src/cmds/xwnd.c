#include <embox/cmd.h>
#include <stdio.h>
#include <vesa.h>
#include <xwnd/xwnd.h>
#include <asm/io.h>

#define	VGA_MISC_WRITE		0x3C2
#define	VGA_MISC_READ		0x3CC

void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i < l; i++) {
		vesa_put_pixel(x + i, y, c);
	}
}

void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i < l; i++) {
		vesa_put_pixel(x, y + i, c);
	}
}

void xwnd_draw_window (struct xwnd_window * wnd) {
	xwnd_draw_vert_line  (wnd->x, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y, wnd->wd, 2);
	xwnd_draw_vert_line  (wnd->x + wnd->wd, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y + wnd->ht, wnd->wd, 2);
}

EMBOX_CMD(main);

int main (int argc, char ** argv) {
	struct xwnd_window wnd = {20, 20, 60, 40};
	vesa_dump_state();
	vesa_demo_graphics();
	vesa_clear_screen();
	xwnd_draw_window(&wnd);
	return 0;
}
