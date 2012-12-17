#ifndef LIB_XWND_WINDOW_H_
#define LIB_XWND_WINDOW_H_

struct display;
struct xwnd_window {
	int x;
	int y;
	int ht;
	int wd;
	struct display *display;
};

#endif
