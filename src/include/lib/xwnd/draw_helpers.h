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

/**
 * draws pixel in a local window coordinates
 *
 * @param wnd window
 * @param x x
 * @param y y
 * @param c colour
 */
extern void xwnd_draw_pixel(const struct xwnd_window *wnd, unsigned x,
		unsigned y, unsigned c);
/**
 * draws horizontal line in a local window coordinates
 *
 * @param wnd window
 * @param x x
 * @param y y
 * @param l length
 * @param c colour
 */
extern void xwnd_draw_horizontal_line(const struct xwnd_window * wnd,
		unsigned x, unsigned y, unsigned l, unsigned c);
/**
 * draws vertical line in a local window coordinates
 *
 * @param wnd window
 * @param x x
 * @param y y
 * @param l length
 * @param c colour
 */
extern void xwnd_draw_vertical_line(const struct xwnd_window * wnd, unsigned x,
		unsigned y, unsigned l, unsigned c);
/**
 * draws line in a local window coordinates
 *
 * @param wnd window
 * @param x1 x1
 * @param y1 y1
 * @param x2 x2
 * @param y2 y2
 * @param c colour
 */
extern void xwnd_draw_line(const struct xwnd_window * wnd, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c);
/**
 * draws rectangle in a local window coordinates
 *
 * @param wnd window
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param c colour
 */
extern void xwnd_draw_rectangle(const struct xwnd_window * wnd, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c);
/**
 * draws polygon in a local window coordinates
 *
 * @param wnd window
 * @param points {x1, y1, x2, y2, ..., xn, yn) array
 * @param count n
 * @param c colour
 */
extern void xwnd_draw_polygon(const struct xwnd_window * wnd, unsigned *points,
		int count, unsigned c);
/**
 * draws ellipse in a local window coordinates. Ellipse position determines by rectangle
 *
 * @param wnd window
 * @param x left coordinate
 * @param y top coordinate
 * @param a width
 * @param b height
 * @param c colour
 */
extern void xwnd_draw_ellipse(const struct xwnd_window * wnd, unsigned x,
		unsigned y, unsigned a, unsigned b, unsigned color);

extern void xwnd_draw_window(const struct xwnd_window *wnd);
extern void xwnd_clear_window(const struct xwnd_window *wnd);

#endif /* XWND_DRAW_HELPERS_H_ */
