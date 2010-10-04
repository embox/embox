/**
 * @file
 * @brief Run elf file from chosen address.
 *
 * @date 21.07.2010
 * @author Avdyukhin Dmitry
 */
#include <shell_command.h>
#include <kernel/scheduler.h>
#include <kernel/elf_executer.h>

#define COMMAND_NAME     "runelf"
#define COMMAND_DESC_MSG "execute elf file"
#define HELP_MSG "Usage: runelf [-h] [-a addr]"

#define THREAD_STACK_SIZE 0x10000

static struct thread *thread;
static char thread_stack[THREAD_STACK_SIZE];
static unsigned int file_addr;

static const char *man_page =
	#include "runelf_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void run(void) {
	elf_execute((FILE *)file_addr);
}

static int exec(int argsc, char **argsv) {
	int op;
	getopt_init();
	do {
		op = getopt(argsc, argsv, "ha:");
		switch(op) {
		case 'a':
			if ((optarg == NULL) || (*optarg == '\0')) {
				TRACE("runelf: missed address value\n");
				return -1;
			}
			if (sscanf(optarg, "0x%x", &file_addr) > 0) {
				run();
				thread = thread_create(run, thread_stack + THREAD_STACK_SIZE);
				thread_start(thread);
				scheduler_start();
				scheduler_stop();
				return 0;
			}
			TRACE("runelf: invalid value \"%s\".\nthe number expected.\n", optarg);
			return -1;
		case 'h':
			show_help();
			return 0;
		case -1:
			return 0;
		default:
			TRACE("runelf: invalid arguments.\n", optarg);
			return -1;
		}
	} while (op != -1);
	return 0;
}
