/**
 * @file
 * @brief Prints threads statistic for Embox
 *
 * @date 2 Dec 2010
 * @author Gleb Efimov
 * @author Alina Kramar
 * @author Roman Oderov
 */

#include <shell_command.h>
#include <kernel/thread.h>
#include <lib/list.h>
#include <stdio.h>
#include <assert.h>

#define COMMAND_NAME     "thread"
#define COMMAND_DESC_MSG "print threads statistic for Embox"
#define HELP_MSG         "Usage: thread [-h] [-k <thread id>] [-s]"
static const char *man_page =
#include "thread_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void print_stat(void) {
	struct thread *thread;
	int run = 0, wait = 0, zombie = 0;
	int total = 0;

	printf(" %3s %4s %6s\n", "Id", "Prio", "State");

	thread_foreach(thread) {
		const char *state;
		assert(thread->exist);

		switch (thread->state) {
		case THREAD_STATE_RUN:
			state = "run";
			run++;
			break;
		case THREAD_STATE_WAIT:
			state = "wait";
			wait++;
			break;
		case THREAD_STATE_ZOMBIE:
			state = "zombie";
			zombie++;
			break;
		default:
			state = "unknown";
			break;
		}
		printf(" %3d %4d %6s\n", thread->id, thread->priority, state);
	}

	total = run + wait + zombie;

	printf("Total: %d threads (%d run,  %d wait, %d zombie)\n", total, run,
			wait, zombie);
}

static void thread_kill(int thread_id) {
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
	int next_opt;

	getopt_init();

	while ((next_opt = getopt(argc, argv, "hsk:")) != -1) {
		printf("\n");
		switch (next_opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			show_help();
			break;
		case 's':
			print_stat();
			break;
		case 'k': {
			int thread_id;

			if ((optarg == NULL) || (!sscanf(optarg, "%d", &thread_id))) {
				show_help();
				break;
			}

			thread_kill(thread_id);
			break;
		}
		default:
			break;
		}
	}

	return 0;
}

