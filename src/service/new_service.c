#include <lib/service/service.h>

#include <string.h>

void * entry_poit (void * arg) {
	struct service_data srv_data;
	struct service_file srv_file;

	service_get_service_data(&srv_data, arg);
	service_file_open_write(&srv_file);

	fprintf(f, "Hello");

	service_file_switch_to_read_mode(&srv_file);
	service_send_reply(&srv_data, &srv_file);

	return NULL;
}
