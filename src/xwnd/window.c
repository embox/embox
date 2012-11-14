/**
 * @file
 * @brief XWnd basic widgets implementation
 *
 * @date Oct 29, 2012
 * @author Alexandr Chernakov
 */
#include <types.h>
#include <xwnd/window.h>

struct xwnd_window *xwnd_window_init(struct xwnd_window * wnd, const struct xwnd_rect * rect) {
	wnd->wdg.type = NULL;
	wnd->wdg.wdg_list = NULL;
	wnd->wdg.next = NULL;
	if (rect) {
		wnd->wdg.rect.x = rect->x;
		wnd->wdg.rect.y = rect->y;
		wnd->wdg.rect.wd = rect->wd;
		wnd->wdg.rect.ht = rect->ht;
	} else {
		wnd->wdg.rect.x = 10;
		wnd->wdg.rect.y = 10;
		wnd->wdg.rect.wd = 100;
		wnd->wdg.rect.ht = 100;
	}
	return wnd;
}

#if 0
void xwnd_window_delete (struct xwnd_window * wnd) {
	if (wnd)
		free (wnd);
}
#endif
