/*
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev
 */

#ifndef WEB_SERVICE_H_
#define WEB_SERVICE_H_

#include <util/array.h>
#include <util/dlist.h>
#include <cmd/servd.h>
#include <lib/service/service.h>
#include <stdlib.h>

#define WEB_SERVISE_MAX_COUNT 128

struct web_service_desc {
	const char *srv_name;
	int *is_started;
	void *(*run)(void *);
};

struct web_service_instance {
	const struct web_service_desc *desc;
	struct service_data *srv_data;
	struct dlist_head lst;
};

#define EMBOX_WEB_SERVICE(name, is_started, thr_handler) \
	ARRAY_SPREAD_DECLARE(const struct web_service_desc, \
			__web_services_repository); \
	ARRAY_SPREAD_ADD(__web_services_repository, {name, is_started, thr_handler})

extern int web_service_add(const char *srv_name);

extern int web_service_stop(const char *srv_name);

extern void web_service_remove_all(void);

extern int is_service_started(const char *srv_name);

extern int web_service_start_service(const char *srv_name,
		struct service_data * srv_data);

#endif /* WEB_SERVICE_H_ */
