/**
 * @file
 * @brief XWnd support for text output
 *
 * @date Nov 7, 2012
 * @author Alexandr Chernakov
 */

#ifndef XWND_FONTS_H_
#define XWND_FONTS_H_

#include <lib/xwnd/draw_helpers.h>

extern void xwnd_print_char (const struct xwnd_window * wnd, unsigned x, unsigned y, char sym);
extern void xwnd_print_text (const struct xwnd_window * wnd, unsigned x, unsigned y, const char * text);

#endif /*XWND_FONTS_H_*/
