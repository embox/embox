/*
 * draw_helpers.h
 *
 *  Created on: 15.10.2012
 *      Author: alexandr
 */

/**
 * @file
 * @brief XWnd library for *.bmp files loading and output
 *
 * @date Oct 3, 2012
 * @author Alexandr Chernakov
 */
#ifndef XWND_DRAW_HELPERS_H_
#define XWND_DRAW_HELPERS_H_

#include <xwnd/window.h>

extern void xwnd_draw_pixel (const struct xwnd_window * wnd, unsigned x, unsigned y, unsigned c);
extern void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c);
extern void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c);

#endif /* XWND_DRAW_HELPERS_H_ */
