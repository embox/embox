/**
 * @file
 * @brief Starts services
 *
 * @date 16.04.16
 * @author Alexander Kalmuk
 */

#include "kernel/task/resource/errno.h"
#include "kernel/task/task_table.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>

#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <framework/mod/options.h>
#include <kernel/task.h>

#define SERVICE_NAME_LEN OPTION_GET(NUMBER,service_name_len)
#define SERVICES_COUNT OPTION_GET(NUMBER,services_count)

struct service_list {
	struct dlist_head dlist_item;
	char service_name[SERVICE_NAME_LEN];
	pid_t pid;
};

POOL_DEF(pool, struct service_list, SERVICES_COUNT);

static DLIST_DEFINE(head);

/* helper to check that service is up running*/
static int is_service_up(struct service_list *item) {
    struct task *tsk = task_table_get(item->pid);

    if (tsk){
        char name_cpy[SERVICE_NAME_LEN];
        char *first_word;
        strcpy(name_cpy, tsk->tsk_name);
        first_word = strtok(name_cpy, " ");
        if(strcmp(first_word, item->service_name) == 0)
            return 1;
    }
    return 0;
}

static void print_usage(void) {
	printf("USAGE: service - print list of available services\n"
			"service [-s stack_size] service_name - start a new service\n"
			"service service_name stop - deactivate the service\n");
}

/* Lookup service node in the list by name */
static struct service_list *service_lookup_by_name(const char *name) {
    struct service_list *tmp;

    dlist_foreach_entry(tmp, &head, dlist_item) {
        if (strcmp(tmp->service_name, name) == 0)
            return tmp;
    }
    return NULL;
}

/* Add service node to running services list */
static void service_add(const char *name, pid_t service_pid) {
  	struct service_list *new_element;

	new_element = pool_alloc(&pool);

	if (new_element == NULL) {
		printf("Could not add service (the pool is full)\n");
		return;
	}

	strncpy(new_element->service_name, name, SERVICE_NAME_LEN - 1);
	new_element->service_name[SERVICE_NAME_LEN - 1] = '\0';
	new_element->pid = service_pid;
  	dlist_init(&new_element->dlist_item);
	dlist_add_next(&new_element->dlist_item, &head);
}

/* Delete service node from running services list */
static void service_delete(struct service_list *service_node) {
    printf("Killing the process:");
    int kill_result = kill(service_node->pid, SIGKILL);
    if (kill_result != 0) {
        printf("error: kill process exited with code %d\n", kill_result);
        printf("Service is probably shut down. Removing from the list anyway\n");
    }
    else
        printf("OK\n");

    printf("Removing from running services list...");
    dlist_del_init(&service_node->dlist_item);
    pool_free(&pool, service_node);
    printf("done\n");
}

/* Print out running services */
static void listprint(void) {
	struct service_list *tmp;

	dlist_foreach_entry(tmp, &head, dlist_item) {
        /* Check that the service is still running, if not remove from list*/
        if (!is_service_up(tmp)) {
            dlist_del_init(&tmp->dlist_item);
            pool_free(&pool, tmp);
            printf("[ Task Id=%d ]\t%s\t [ exited ]\n", tmp->pid, tmp->service_name);
        } else {
            printf("[ Task Id=%d ]\t%s\n", tmp->pid, tmp->service_name);
        }
	}
}

/* Run given service */
static int service_run(const char *path, char *const argv[]) {
	pid_t pid;
    int res = 0;

	pid = vfork();

	if (pid < 0) {
		int err = errno;
		printf("vfork() error(%d): %s", err, strerror(err));
		return -err;
	}

    if (pid == 0) {
        /* That is considered a bad manner to have child altering any variables,
         * except for pid, since it shares memory space with the parent.
         * However, without fork() and wait() we have to manage somehow.
         * Until a better solution is around, this is the option we can use, to
         * understand if the child successfully launched needed service. 
         * Probably needs a fix in future*/
        res = execv(path, argv);
        exit(1);
    }
    else {
        if(res < 0) {
            printf("service: No such executable\n");
            return -1;
        }
        service_add(path, pid);
    }
    return 0;
}

/* Service.c: Main */
int main(int argc, char **argv) {
	int opt;
    struct service_list *service_node = NULL;
	const char *command;
    int arg_offset = 1;

	if (argc == 0) {
		return -EINVAL;
	}

	if (argv[1] == NULL) {
		listprint();
		return 0;
	}

	opt = getopt(argc, argv, "s:h");
	switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's': {
			int stack_size;
			struct rlimit rlp;

			if (!optarg) {
				print_usage();
				return -1;
			}
			stack_size = (int) strtoul(optarg, NULL, 0);
			rlp.rlim_max = stack_size;
			rlp.rlim_cur = stack_size;
			setrlimit(RLIMIT_STACK, &rlp);
			printf("Service will be launched with %d stack size\n", stack_size);
			arg_offset = 3;
			break;
		}
	}

	command = argv[arg_offset];

    /* Stop command */
    if (arg_offset+1 < argc)
        if (strcmp(argv[arg_offset+1], "stop") == 0) {
            service_node = service_lookup_by_name(command);
            if (service_node){
                /* Is the task still up or it has exited since last start */
                if (!is_service_up(service_node)) {
                    dlist_del_init(&service_node->dlist_item);
                    pool_free(&pool, service_node);
                    printf("Service has exited\n");
                } else { /* If up then stop */
                    service_delete(service_node);
                }
            } else {
                printf("Cannot stop %s, no such service running\n", command);
            }
            return 0;
        }

    /* Start process */
    service_node = service_lookup_by_name(command);
    if (service_node) {
        if (is_service_up(service_node)) {
            printf("Service is already running\n");
            return 0;
        } else { /* exited since last start */
            dlist_del_init(&service_node->dlist_item);
            pool_free(&pool, service_node);
            printf("Service has previously exited...restarting\n");
        }
    }
    /* Normal starting routine */
    argv[argc] = NULL;
    printf("Starting service: %s\n", command);

    if(service_run(command, &argv[arg_offset]) < 0)
        printf("Couldn't start service %s\n", argv[1]);
    else
        printf("Successfully started\n");
    return 0;
}
