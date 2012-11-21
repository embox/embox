/**
 * @file
 *
 * @date Nov 14, 2012
 * @author: Anton Bondarev
 */

#include <mem/misc/pool.h>
#include <xwnd/window.h>

POOL_DEF(window_pool, struct xwnd_window, sizeof(struct xwnd_window));

struct xwnd_window *xwnd_window_alloc(void) {
	return pool_alloc(&window_pool);
}

void xwnd_window_free(struct xwnd_window *wnd) {
	pool_free(&window_pool, wnd);
}
