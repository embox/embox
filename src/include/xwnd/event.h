/*
 * event.h
 *
 *  Created on: 15.10.2012
 *      Author: alexandr
 */

#ifndef XWND_EVENT_H_
#define XWND_EVENT_H_

#include <kernel/thread/event.h>

enum xwnd_event_type {XWND_SIGNALLED, XWND_KILLED, XWND_KBD_PRESS, XWND_REDRAW /*to be continued*/};
struct xwnd_kbd_event {
	char key; /*Oh, forgive me, mom*/
};
struct xwnd_mouse_event {
	int x;
	int y;
	int state;
};
union xwnd_event_info {
	struct xwnd_kbd_event kbd;
	struct xwnd_mouse_event mouse;
};
struct xwnd_event {
	enum xwnd_event_type type;
	struct event * krnl_event;
	union xwnd_event_info * info;
	struct xwnd_event * next;
};
typedef struct xwnd_event * xwnd_event_list;
typedef void (*xwnd_event_handler) (struct xwnd_event * event);

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

struct xwnd_event_pipe {
	int input;
	int output;
};
struct xwnd_event_pipes_array {
	int count;
	int allocated;
	struct xwnd_event_pipe * array;
};

struct xwnd_event_pipes_array * xwnd_event_pipes_init(void);
const struct xwnd_event_pipe * xwnd_event_pipe_add(void);
void xwnd_event_pipe_delete(void);


#endif /* XWND_EVENT_H_ */
