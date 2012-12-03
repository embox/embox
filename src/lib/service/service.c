/**
 * @file
 *
 * @date Dec 3, 2012
 * @author: Alexandr Chernakov, Vita Loginova
 */

#include <lib/service/service.h>
#include <util/rand.h>
#include <stdlib.h>
#include <string.h>

#define SERVICE_FILE_RAND_STR_LEN 10
#define SERVICE_FILE_PREFIX "/tmp/service-"
#define SERVICE_FILE_POSTFIX "-temp.html"

static char rand_letter (void) {
	return 'a' + (rand() % 26);
}

int service_file_open_write(struct service_file *srv_file) {
	int i;
	int postfix_len = strlen(SERVICE_FILE_POSTFIX);
	int prefix_len = strlen(SERVICE_FILE_PREFIX);
	int f_name_len = prefix_len + SERVICE_FILE_RAND_STR_LEN + postfix_len + 1;

	srv_file->name = malloc(f_name_len);
	if (!srv_file->name) {
		return -1;
	}
	srv_file->name[0] = '\0';
	strcat (srv_file->name, SERVICE_FILE_PREFIX);
	for (i = strlen(SERVICE_FILE_PREFIX); i < SERVICE_FILE_RAND_STR_LEN + prefix_len; i++) {
		srv_file->name[i] = rand_letter();
	}
	srv_file->name[i] = '\0';
	strcat (srv_file->name, SERVICE_FILE_POSTFIX);

	srv_file->fd = fopen(srv_file->name, "w");
	if (!srv_file->fd) {
		service_file_close(srv_file);
		return -1;
	}

	return 0;
}

int service_file_switch_to_read_mode(struct service_file *srv_file) {
	if (srv_file->fd) {
		fclose(srv_file->fd);
	} else {
		service_file_close(srv_file);
		return -1;
	}
	srv_file->fd = fopen(srv_file->name, "r");
	if (!srv_file->fd) {
		service_file_close(srv_file);
		return -1;
	}
	return 0;
}

void service_file_close(struct service_file *srv_file) {
	if (srv_file->name)
		free(srv_file->name);
	if (srv_file->fd)
		fclose(srv_file->fd);
}
