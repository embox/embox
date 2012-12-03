#include <lib/service/service.h>
#include <embox/web_service.h>

#include <string.h>
#include <stdio.h>

void * entry_point (void * arg) {
	struct service_data srv_data;
	struct service_file srv_file;

	service_get_service_data(&srv_data, arg);
	service_file_open_write(&srv_file);

	fprintf(srv_file.fd, "Hello");

	service_file_switch_to_read_mode(&srv_file);
	service_send_reply(&srv_data, &srv_file);

	return NULL;
}

EMBOX_WEB_SERVICE("new.html", entry_point);
