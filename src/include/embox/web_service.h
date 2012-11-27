/*
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev
 */

#ifndef WEB_SERVICE_H_
#define WEB_SERVICE_H_

#include <kernel/thread/event.h>
#include <util/array.h>
#include <util/dlist.h>
#include <cmd/servd.h>


struct web_service_desc {
	const char *srv_name;
	void *(*run)(void *);
};

struct web_service_instance {
	struct thread *thr;
	struct event *e;
	struct params * params;
	const struct web_service_desc *desc;
	struct dlist_head lst;
};

extern const struct web_service_desc __web_services_repository[];

#define EMBOX_WEB_SERVICE(name, thr_handler) \
	ARRAY_SPREAD_ADD(__web_services_repository, {name,thr_handler})


extern int web_service_start(const char *srv_name);

extern int web_service_send_message(const char *srv_name, void *par);

extern int web_service_stop(const char *srv_name);


#endif /* WEB_SERVICE_H_ */
