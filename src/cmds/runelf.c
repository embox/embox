/**
 * @file
 * @brief Run ELF file.
 *
 * @date 21.07.10
 * @author Avdyukhin Dmitry
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <kernel/thread/sched.h>
#include <lib/libelf.h>
#include <stdio.h>
#include <fs/file.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: runelf [-h] [-f file]\n");
}
#define THREAD_STACK_SIZE 0x10000

//static struct thread *thread;
//static char thread_stack[THREAD_STACK_SIZE];
static unsigned int file_addr;

static void *run(void *arg) {
	printf("run addr = 0x%X\n", file_addr);
	elf_execve((unsigned long *) file_addr, NULL);

	return NULL;
}

static int exec(int argc, char **argv) {
	int opt;
	char *file_name = NULL;
	FILE *file;
	getopt_init();
	do {
		opt = getopt(argc, argv, "f:h");
		switch(opt) {
		case 'f':
			file_name = optarg;
			break;
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return -1;
		}
	} while (opt != -1);

	if (NULL == file_name) {
		TRACE("\n please setup file name\n");
		return 0;
	}
	file = fopen(file_name, "r");
	if (NULL == file) {
		TRACE("\nCan't open file %s\n", file_name);
		return 0;
	}
	fioctl(file, 0, &file_addr);

	run(NULL);
//	thread = thread_init(run, thread_stack, THREAD_STACK_SIZE);
//	thread_start(thread);
	fclose(file);
	return 0;
}
