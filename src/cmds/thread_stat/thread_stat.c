/**
 * @file
 * @brief Prints threads statistic for EMBOX
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
#define COMMAND_DESC_MSG "print threads statistic for EMBOX"
#define HELP_MSG         "Usage: thread [-h] [-k <thread id>] [-s]"
static const char *man_page =
#include "thread_stat_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static void print_stat(void) {
	struct thread *thread;
	int run = 0, wait = 0, zombie = 0;

	thread_foreach(thread) {
		assert(thread->exist);
		switch (thread->state) {
		case THREAD_STATE_RUN:
			run++;
			break;
		case THREAD_STATE_WAIT:
			wait++;
			break;
		case THREAD_STATE_ZOMBIE:
			zombie++;
			break;
		default:
			break;
		}
		TRACE("\tTHREAD id : %d; priority : %d; \n", thread->id, thread->priority);
	}

	TRACE("THREAD_STATE_RUN  %d\n", run);
	TRACE("THREAD_STATE_WAIT  %d\n", wait);
	TRACE("THREAD_STATE_ZOMBIE  %d\n", zombie);
}

static void thread_kill(int thread_id) {
	if (thread_id != -1) {

		struct thread *thread;

		if (thread_id < 0) {
			printf("Invalid (negative) thread id: %d\n", thread_id);

		} else {
			thread = thread_get_by_id(thread_id);

			if (thread == NULL) {
				printf("No thread with id: %d\n", thread_id);
			} else if (thread == idle_thread) {
				printf("Can't kill idle thread\n");
			} else {
				int error = thread_stop(thread);

				if (!error) {
					printf("Thread %d killed\n", thread_id);
				} else {
					printf("Unable to kill thread %d: %s\n", thread_id,
							strerror(error));
				}
			}
		}
	}
}

static int exec(int argc, char **argv) {
	int next_opt;
	int thread_id = -1;

	getopt_init();

	while ((next_opt = getopt(argc, argv, "hsk:")) != -1) {
		switch (next_opt) {
		case '?':
		case 'h':
			show_help();
			break;
		case 's':
			print_stat();
			break;
		case 'k':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &thread_id))) {
				show_help();
				return -1;
			} else {
				thread_kill(thread_id);
				return 0;
			}
			break;
		default:
			return 0;
		};
	}

	return 0;
}

