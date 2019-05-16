/**
 * @file
 * @brief Starts services
 *
 * @date 16.04.16
 * @author Alexander Kalmuk
 */
#include <util/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <sys/types.h>
#include <signal.h>
#include <framework/mod/options.h>

#define SERVICE_NAME_LEN OPTION_GET(NUMBER,service_name_len)
#define SERVICES_COUNT OPTION_GET(NUMBER,services_count)

struct service_list {
	struct dlist_head dlist_item;
	char service_name[SERVICE_NAME_LEN];
	pid_t pid;
};

POOL_DEF(pool, struct service_list, SERVICES_COUNT);

static DLIST_DEFINE(head);

static void service_add(const char *name, pid_t service_pid) {
  	struct service_list *new_element;
	struct service_list *tmp;

	dlist_foreach_entry(tmp, &head, dlist_item) {
		if (strcmp(tmp->service_name, name) == 0) {
			printf("Service is already running.\n");
			return;
		}
	}

	new_element = pool_alloc(&pool);

	if (new_element == NULL) {
		log_error("Pool is full.");
		return;
	}

	strncpy(new_element->service_name, name, SERVICE_NAME_LEN - 1);
	new_element->service_name[SERVICE_NAME_LEN - 1] = '\0';

	new_element->pid = service_pid;

  	dlist_init(&new_element->dlist_item);

	dlist_add_next(&new_element->dlist_item, &head);
}

static void service_delete(const char *name) {
	struct service_list *tmp;

	dlist_foreach_entry(tmp, &head, dlist_item) {
		if (strcmp(tmp->service_name, name) == 0) {
			int kill_result = kill(tmp->pid, SIGKILL);

			if (kill_result != 0) {
				printf("Error: the service was launched but is not working now.\n");
				return;
			}

			dlist_del_init(&tmp->dlist_item);
			pool_free(&pool, tmp);

			printf("Successful service shutdown.\n");
			return;
		}
	}
	printf("This service doesn't work.\n");
	return;
}

static void listprint(void) {
	struct service_list *tmp;

	dlist_foreach_entry(tmp, &head, dlist_item) {
		printf("%s\n", tmp->service_name);
	}
}

static int service_run(const char *path, char *const argv[]) {
	pid_t pid;
	pid = vfork();

	if (pid < 0) {
		int err = errno;
		printf("vfork() error(%d): %s", err, strerror(err));
		return -err;
	}

	if (pid == 0) {
		execv(path, argv);
		exit(1);
	}
	service_add(path, pid);

	return 0;
}

int main(int argc, char **argv) {
	const char *command;

	if (argc == 0) {
		return -EINVAL;
	}

	if (argv[1] == NULL) {
		listprint();
		return 0;
	}

	command = argv[1];

	if (argc > 2 && strcmp(argv[2], "stop") == 0) {
		service_delete(command);
		return 0;
	}

	argv[argc] = NULL;
	printf("Starting service: %s\n", command);

	return service_run(command, &argv[1]);
}
