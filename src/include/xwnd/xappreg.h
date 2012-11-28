#ifndef XAPPREG_H_
#define XAPPREG_H_

#include <lib/x_event.h>
#include <lib/xwnd/application.h>
#include <util/array.h>

#define MAX_XWND_APPS 256

struct xwnd_app_desc {
	const char *app_name;
	void *(*run)(void *);
};

struct xwnd_app_registry_node {
	int thread_id;
	struct x_event_master * xev_master;
	int empty;
};

//extern int xwnd_app_registry_init (void); //
//extern void xwnd_app_registry_fini (void);  //

extern int xwnd_app_registry_allocate (void);//
extern void xwnd_app_registry_free (int xapp_id);//

extern int xwnd_app_registry_get_subscription (struct xwnd_application * xapp);
extern void xwnd_app_registry_release_subscription (int xapp_id); //

extern int xwnd_app_registry_move_focus (void);
extern void xwnd_app_registry_broadcast_event (struct x_event * ev); //
extern void xwnd_app_registry_broadcast_quit_event (void);
extern int xwnd_app_registry_send_event (int xapp_id, struct x_event * ev); //

extern void xwnd_app_registry_send_sys_event (enum x_event_type type);
extern void xwnd_app_registry_send_kbd_event (char key);
extern int xwnd_app_registry_send_event_focused (struct x_event * ev);

extern int xwnd_app_start(const char *app_name, char *arg);

extern const struct xwnd_app_desc __xwnd_app_repository[];

#define EMBOX_XWND_APP(name, app_handler) \
	ARRAY_SPREAD_ADD(__xwnd_app_repository, {name,app_handler})

#endif
