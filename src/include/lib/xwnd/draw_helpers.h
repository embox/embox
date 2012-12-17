/**
 * @file
 * @brief XWnd library for *.bmp files loading and output
 *
 * @date Oct 3, 2012
 * @author Alexandr Chernakov
 */
#ifndef XWND_DRAW_HELPERS_H_
#define XWND_DRAW_HELPERS_H_
#if 0
#include <xwnd/window.h>
#endif
#include <lib/xwnd/window.h>

extern void xwnd_draw_pixel(const struct xwnd_window *wnd, unsigned x,
		unsigned y, unsigned c);
extern void xwnd_draw_horizontal_line(const struct xwnd_window * wnd,
		unsigned x, unsigned y, unsigned l, unsigned c);
extern void xwnd_draw_vertical_line(const struct xwnd_window * wnd, unsigned x,
		unsigned y, unsigned l, unsigned c);
extern void xwnd_draw_line(const struct xwnd_window * wnd, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c);
extern void xwnd_draw_rectangle(const struct xwnd_window * wnd, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c);
extern void xwnd_draw_polygon(const struct xwnd_window * wnd, unsigned *points,
		int count, unsigned c);
extern void xwnd_draw_ellipse(const struct xwnd_window * wnd, unsigned x, unsigned y,
		unsigned a, unsigned b, unsigned color);

extern void xwnd_draw_window(const struct xwnd_window *wnd);
extern void xwnd_clear_window(const struct xwnd_window *wnd);

#endif /* XWND_DRAW_HELPERS_H_ */
