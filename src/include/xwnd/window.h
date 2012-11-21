/**
 * @file
 * @brief XWnd basic widgets
 *
 * @date Oct 15, 2012
 * @author Alexandr Chernakov
 */

#ifndef XWND_WINDOW_H_
#define XWND_WINDOW_H_

struct xwnd_widget;

struct xwnd_widget_type {
	void (*draw_wdg_fun)(struct xwnd_widget *);
};

extern int xwnd_register_widget_type (struct xwnd_widget_type * xwndwtype);
extern void xwnd_unregister_widget_type (struct xwnd_widget_type * xwndwtype);

struct xwnd_point {
	int x;
	int y;
};

struct xwnd_rect {
	int x;
	int y;
	int ht;
	int wd;
};

struct xwnd_widget {
	struct xwnd_widget_type * type;
	struct xwnd_rect rect;
	struct xwnd_widget * wdg_list;
	struct xwnd_widget * next;
};

struct xwnd_window {
	struct xwnd_widget wdg;
	char * caption;
};

struct xwnd_button {
	struct xwnd_widget wdg;
	char * text;
};

extern struct xwnd_window * xwnd_window_create (const struct xwnd_rect * rect);
extern void xwnd_window_delete (struct xwnd_window * wnd);
extern void xwnd_draw_window (struct xwnd_window * wnd);

#endif /* XWND_WINDOW_H_ */
