/**
 * @file
 * @brief XWnd basic widgets implementation
 *
 * @date Oct 29, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/window.h>
#include <stdlib.h>

struct xwnd_window * xwnd_window_create (const struct xwnd_rect * rect) {
	struct xwnd_window * tmp;
	tmp = malloc(sizeof(struct xwnd_window));
	if (!tmp) {
		return NULL;
	}
	tmp->wdg.type = NULL;
	tmp->wdg.wdg_list = NULL;
	tmp->wdg.next = NULL;
	if (rect) {
		tmp->wdg.rect.x = rect->x;
		tmp->wdg.rect.y = rect->y;
		tmp->wdg.rect.wd = rect->wd;
		tmp->wdg.rect.ht = rect->ht;
	} else {
		tmp->wdg.rect.x = 10;
		tmp->wdg.rect.y = 10;
		tmp->wdg.rect.wd = 100;
		tmp->wdg.rect.ht = 100;
	}
	return tmp;
}

void xwnd_window_delete (struct xwnd_window * wnd) {
	if (wnd)
		free (wnd);
}
