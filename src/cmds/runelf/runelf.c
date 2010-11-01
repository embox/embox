/**
 * @file
 * @brief Run ELF file.
 *
 * @date 21.07.2010
 * @author Avdyukhin Dmitry
 */
#include <shell_command.h>
#include <kernel/scheduler.h>
#include <lib/libelf.h>
#include <stdio.h>
#include <fs/file.h>

#define COMMAND_NAME     "runelf"
#define COMMAND_DESC_MSG "execute elf file"
#define HELP_MSG "Usage: runelf [-h] [-f file]"

#define THREAD_STACK_SIZE 0x10000

static struct thread *thread;
static char thread_stack[THREAD_STACK_SIZE];
static unsigned int file_addr;

static const char *man_page =
	#include "runelf_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void run(void) {
	elf_execve((unsigned long *)file_addr, NULL);
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	char *file_name;
	FILE *file;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hf:");
		switch(nextOption) {
		case 'f':
			file_name = optarg;
			break;
		case 'h':
			show_help();
			return 0;
		case -1:
			return 0;
		default:
			return -1;
		}
	} while (nextOption != -1);

	file = fopen(file_name, "r");
	fioctl(file, 0, &file_addr);

	run();
	thread = thread_create(run, thread_stack + THREAD_STACK_SIZE);
	thread_start(thread);
	scheduler_start();
	scheduler_stop();

	return 0;
}
