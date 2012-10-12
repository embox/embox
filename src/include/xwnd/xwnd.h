/*
 * xwnd.h
 *
 *  Created on: Oct 1, 2012
 *      Author: user
 */

#ifndef XWND_H_
#define XWND_H_

#include <kernel/thread/event.h>

enum xwnd_event_type {XWND_SIGNALLED, XWND_KILLED, XWND_KBD_PRESS, XWND_REDRAW /*to be continued*/};

struct xwnd_kbd_event {
	int key;
};

struct xwnd_mouse_event {
	int x;
	int y;
};

union xwnd_event_info {
	struct xwnd_kbd_event kbd;
	struct xwnd_mouse_event mouse;
};

struct xwnd_event {
	enum xwnd_event_type type;
	struct event * krnl_event;
	struct xwnd_event_node * next;
};

typedef struct xwnd_event * xwnd_event_list;

typedef void (*xwnd_event_handler) (struct xwnd_event * event, void * reserved);

struct xwnd_window {
	int x;
	int y;
	int wd;
	int ht;
	xwnd_event_list event_list;
};

struct xwnd_app_registry_node {
};

struct xwnd_app_registry {
	unsigned int size;
	unsigned int allocated;
	struct xwnd_app_registry_node * xwnd_app_array;
};

void xwnd_app_registry_init (void);
void xwnd_app_registry_remove (void);
struct xwnd_window * xwnd_new_window (void);
void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c);
void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c);
void xwnd_draw_window (struct xwnd_window * wnd);
int xwnd_set_event_handler (struct xwnd_window * wnd, enum xwnd_event_type ev_type, xwnd_event_handler ev_hndl);
int xwnd_send_event (struct xwnd_window * wnd, struct xwnd_event ev);

#endif /* XWND_H_ */
