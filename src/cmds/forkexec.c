/**
 * @file
 * @brief
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <kernel/task.h>

EMBOX_CMD(exec);

static void *new_task_entry(void *file);

static int exec(int argc, char **argv) {
	char *filename = malloc(strlen(argv[argc-1]));
	strcpy(filename, argv[argc - 1]);
	return new_task(filename, new_task_entry, filename);
}

static void *new_task_entry(void *filename) {
	char s_filename[255];
	char *argv[2] = {s_filename, NULL};
	char *envp[1] = {NULL};

	/* Copying and free filename */
	strcpy(s_filename, filename);
	free(filename);

	execve(s_filename, argv, envp);

	return NULL;
}
