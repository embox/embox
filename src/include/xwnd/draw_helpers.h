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

struct xwnd_window;

extern void xwnd_draw_pixel (const struct xwnd_window *wnd, unsigned x, unsigned y, unsigned c);

extern void xwnd_draw_window (struct xwnd_window *wnd);

#endif /* XWND_DRAW_HELPERS_H_ */
