/**
 * @file esh.c
 *
 * @brief simple embox shell
 *
 * @date 13.11.2010
 * @author Fedor Burdun
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <shell_command.h>
#include <lib/readline.h>
#include <stdio.h>

/* TO-DO add parse argument */

static int esh_start(void) {

	int ret_code;
	SHELL_COMMAND_DESCRIPTOR *scd;
	printk("\nModule `esh' started.\n");
	printf("Printf test\n");

	char *line;

	FILE *ff = fopen("/dev/uart","r");

	for (;;) {
		/*cmd_ptr = __readline(promt_ptr); */
		line = readline(CONFIG_SHELL_PROMPT);

		//TRACE("\nreceived '%s' \n", line);
		printf("\nreceived '%s' \n", line);

		scd = shell_command_descriptor_find_first(line, -1);

		if ((scd!=NULL)&&(scd->name!=NULL)) {
				//TRACE("CmdLine: %s, RunCommand: %s\n",line , scd->name);
				printf("CmdLine: %s, RunCommand: %s\n\n",line , scd->name);
		}

		ret_code = shell_command_exec(scd, 0, NULL);

		//TRACE("return code: %d\n",ret_code);
		printf("return code: %d\n",ret_code);

		freeline(line);
	}

	fclose(ff);

	return 0;
}

static int esh_stop(void) {
	return 0;
}

EMBOX_UNIT(esh_start, esh_stop);

/* tmp */

