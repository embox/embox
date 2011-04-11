/**
 * @file
 * @brief Prints threads statistic for Embox
 *
 * @date 2 Dec 2010
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

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: thread [-h] [-s] [-k <thread_id>]\n");
}

static void print_stat(void) {
	struct thread *thread;
	int running, sleeping, suspended, sleeping_suspended;
	int total;

	printf(" %4s %8s %18s\n", "Id", "Priority", "State");

	running = sleeping = suspended = sleeping_suspended = 0;

	thread_foreach(thread) {
		const char *state;

		switch (thread->state) {
		case THREAD_STATE_RUNNING:
			state = "running";
			running++;
			break;
		case THREAD_STATE_SLEEPING:
			state = "sleeping";
			sleeping++;
			break;
		case THREAD_STATE_SLEEPING_SUSPENDED:
			state = "sleeping_suspended";
			sleeping_suspended++;
			break;
		case THREAD_STATE_SUSPENDED:
			state = "suspended";
			suspended++;
			break;
		default:
			state = "unknown";
			break;
		}
		printf(" %4d %8d %18s\n", thread->id, thread->priority, state);
	}

	total = running + sleeping + suspended + sleeping_suspended;

	printf("Total %d threads: \n"
		"\t%d running\n"
		"\t%d sleeping\n"
		"\t%d suspended\n"
		"\t%d sleeping_suspended\n", total, running, sleeping,
			suspended, sleeping_suspended);
}

static void kill_thread(int thread_id) {
	struct thread *thread;
	int error;

	if (thread_id < 0) {
		printf("Invalid (negative) thread id: %d\n", thread_id);
		return;
	}

	if (!(thread = thread_lookup(thread_id))) {
		printf("No thread with id: %d\n", thread_id);
		return;
	}

#if 0
	if (thread == idle_thread) {
		printf("Can't kill idle thread\n");
		return;
	}
#endif

	if ((error = thread_stop(thread))) {
		printf("Unable to kill thread %d: %s\n", thread_id, strerror(error));
		return;
	}

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
			int thread_id;
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

