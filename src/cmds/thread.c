/**
 * @file
 * @brief Prints threads statistic for Embox
 *
 * @date 02.12.10
 * @author Gleb Efimov
 * @author Alina Kramar
 * @author Roman Oderov
 */

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/task.h>
#include <kernel/sched.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: thread [-h] [-s] [-k <thread_id>]\n");
}

static void print_stat(void) {
	struct thread *thread, *tmp;
	int running, sleeping, suspended;
	int total;

	printf(" %4s  %4s  %8s %18s %10s\n", "id", "tid", "priority", "state", "time");

	running = sleeping = suspended = 0;

	sched_lock();
	{
		thread_foreach(thread, tmp) {
			thread_state_t s = thread->state;
			const char *state = NULL;

			if (thread_state_running(s)) {
				state = "running";
				running++;
			} else if (thread_state_sleeping(s)) {
				state = "sleeping";
				sleeping++;
			}

			printf(" %4d%c %4d  %8d %18s %9lds\n",
				thread->id, thread_state_oncpu(thread->state) ? '*' : ' ',
				thread->task->tid,
				thread_priority_get(thread),
				state,
				thread_get_running_time(thread)/CLOCKS_PER_SEC);
		}
	}
	sched_unlock();
	total = running + sleeping;

	printf("Total %d threads: \n"
		"\t%d running\n"
		"\t%d sleeping\n", total, running, sleeping);
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
		return -EINVAL;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hsk:"))) {
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
