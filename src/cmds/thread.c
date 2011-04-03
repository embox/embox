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
	int running = 0, wait = 0, terminate = 0, susp = 0, wait_susp = 0;
	int total = 0;

	printf(" %10s %4s %10s\n", "Id", "Prio", "State");

	thread_foreach(thread) {
		const char *state;

		switch (thread->state) {
		case THREAD_STATE_RUNNING:
			state = "running";
			running++;
			break;
		case THREAD_STATE_SLEEPING:
			state = "wait";
			wait++;
			break;
		case THREAD_STATE_SLEEPING_SUSPENDED:
			state = "wait_susp";
			wait_susp++;
			break;
		case THREAD_STATE_SUSPENDED:
			state = "susp";
			susp++;
			break;
		case THREAD_STATE_TERMINATE:
			state = "terminate";
			terminate++;
			break;
		default:
			state = "unknown";
			break;
		}
		printf(" %10d %4d %10s\n", thread->id, thread->priority, state);
	}

	total = running + wait + terminate + susp + wait_susp;

//	printf("Total: %d threads (%d run,  %d wait, %d zombie)\n", total, run,
//			wait, zombie);
}

static void kill_thread(int thread_id) {
	struct thread *thread;
	int error;

	if (thread_id < 0) {
		printf("Invalid (negative) thread id: %d\n", thread_id);
		return;
	}

	if ((thread = thread_get_by_id(thread_id)) == NULL) {
		printf("No thread with id: %d\n", thread_id);
		return;
	}

	if (thread == idle_thread) {
		printf("Can't kill idle thread\n");
		return;
	}

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

