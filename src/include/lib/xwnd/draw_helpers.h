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

extern void xwnd_draw_pixel (const struct xwnd_window *wnd, unsigned x, unsigned y, unsigned c);

extern void xwnd_draw_window (const struct xwnd_window *wnd);
extern void xwnd_clear_window (const struct xwnd_window *wnd);

#endif /* XWND_DRAW_HELPERS_H_ */
