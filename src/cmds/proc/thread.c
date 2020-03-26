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
#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/thread_stack.h>

static void print_usage(void) {
	printf("Usage: thread [-h] [-s] [-k <thread_id>]\n");
}

static void print_stat(void) {
	struct thread *t;
	int running, sleeping, suspended;
	int total;
	struct task *task;

	printf(" %4s %16s %8s %7s %10s %12s\n", "id", "tid/Task name", "priority", "state", "time", "Stack size");

	running = sleeping = suspended = 0;

	sched_lock();
	{
		task_foreach(task) {
			task_foreach_thread(t, task) {
				printf(" %4d %4d/%-11s %8d %c %c %c %c %9lds %9zu\n",
					t->id, task_get_id(t->task),
					task_get_name(t->task),
					schedee_priority_get(&t->schedee),
					(t == thread_self()) ? '*' : ' ',
					sched_active(&t->schedee) ? 'A' : ' ',
					t->schedee.ready        ? 'R' : ' ',
					t->schedee.waiting      ? 'W' : ' ',
					thread_get_running_time(t)/CLOCKS_PER_SEC,
					thread_stack_get_size(t));

				if (t->schedee.ready || sched_active(&t->schedee))
					running++;
				else
					sleeping++;
			}
		}
	}
	sched_unlock();

	total = running + sleeping;

	printf("Total %d threads: \n"
		"\t%d running\n"
		"\t%d sleeping\n", total, running, sleeping);
}

static struct thread *thread_lookup(thread_id_t id) {
	struct thread *t;
	struct task * task;

	sched_lock();
	{
		task_foreach(task) {
			task_foreach_thread(t, task) {
				if (t->id == id) {
					goto out;
				}
			}
		}
		t = NULL;
	}
out:
	sched_unlock();

	return t;
}

static void kill_thread(thread_id_t thread_id) {
	struct thread *thread;

	if (!(thread = thread_lookup(thread_id))) {
		printf("No thread with id: %d\n", thread_id);
		return;
	}

	printf("Thread %d killed\n", thread_id);
}

int main(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}


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
