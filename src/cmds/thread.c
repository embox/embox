/**
 * @file
 * @brief Prints threads statistic for Embox
 *
 * @date 02.12.10
 * @author Gleb Efimov
 * @author Alina Kramar
 * @author Roman Oderov
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <kernel/thread/api.h>
#include <kernel/thread/state.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: thread [-h] [-s] [-k <thread_id>]\n");
}

static void print_stat(void) {
	struct thread *thread;
	int running, sleeping, suspended;
	int total;

	printf(" %4s  %8s %18s\n", "Id", "Priority", "State");

	running = sleeping = suspended = 0;

	thread_foreach(thread) {
		thread_state_t s = thread->state;
		const char *state = NULL;

		if (thread_state_running(s)) {
			state = "running";
			running++;
		} else {
			if (thread_state_sleeping(s)) {
				state = "sleeping";
				sleeping++;
			}
			if (thread_state_suspended(s)) {
				state = state ? "sleeping_suspended" : "suspended";
				suspended++;
			}
		}

		printf(" %4d%c %8d %18s\n", thread->id,
				thread == thread_self() ? '*' : ' ', thread->priority, state);
	}

	total = running + sleeping + suspended;

	printf("Total %d threads: \n"
		"\t%d running\n"
		"\t%d sleeping\n"
		"\t%d suspended\n", total, running, sleeping, suspended);
}

static void kill_thread(thread_id_t thread_id) {
	struct thread *thread;
#if 0
	int error;
#endif

	if (!(thread = thread_lookup(thread_id))) {
		printf("No thread with id: %d\n", thread_id);
		return;
	}

#if 0
	if (thread == idle_thread) {
		printf("Can't kill idle thread\n");
		return;
	}

	if ((error = thread_stop(thread))) {
		printf("Unable to kill thread %d: %s\n", thread_id, strerror(error));
		return;
	}
#endif

	printf("Thread %d killed\n", thread_id);
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	getopt_init();

	while ((opt = getopt(argc, argv, "hsk:")) != -1) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 's':
			print_stat();
			break;
		case 'k': {
			thread_id_t thread_id;
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &thread_id))) {
				print_usage();
				break;
			}
			kill_thread(thread_id);
			break;
		}
		default:
			break;
		}
	}

	return 0;
}
