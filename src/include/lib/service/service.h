/**
 * @file
 *
 * @date Dec 3, 2012
 * @author: Alexandr Chernakov, Vita Loginova
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <net/util/request_parser.h>

struct service_data{
	char *query;
	int sock;
	struct http_request request;
};

struct service_file{
	FILE *fd;
	char *name;
};

extern struct service_data *service_get_service_data(void* arg);

extern int service_file_allocate(struct service_file *srv_file);

extern int service_file_switch_to_read_mode(struct service_file *srv_file);

extern int service_send_reply(struct service_data *srv_data, struct service_file *srv_file);

#endif /* SERVICE_H_ */
