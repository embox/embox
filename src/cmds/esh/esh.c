
#include <embox/unit.h>
#include <kernel/printk.h>
#include <shell_command.h>
#include <lib/readline.h>
#include <stdio.h>

#if 0
#define USE_READLINE
//#define USE_FOPEN

#ifdef USE_READLINE
#else
#endif

#ifdef USE_FOPEN
#include <fs/file.h>
#endif

#ifdef USE_READLINE
const char* cmd_1 = "help";
const char* cmd_2 = "ls";

char * __readline(char *arg) {
	return cmd_2;
}
#endif
#endif

static int esh_start(void) {
#if 0
	char cmd[255];
	char promt[255];
	char *cmd_ptr = &cmd[0];
	char *promt_ptr = &promt[0];
	promt_ptr = ">";
#endif

	int ret_code;
	SHELL_COMMAND_DESCRIPTOR *scd;
	printk("\nModule `esh' started.\n");
	printf("Printf test\n");

	char *line;

	FILE *ff = fopen("/dev/uart","r");

	for (;;) {
		/*cmd_ptr = __readline(promt_ptr); */
		line = readline(CONFIG_SHELL_PROMPT);
		shell_command_descriptor_find_first(line, -1);

		#if 1
			printk("CmdLine: %s, RunCommand: %s\n",line , scd->name);
		#endif

		ret_code = shell_command_exec(scd, 0, NULL);

		freeline(line);
	}

	fclose(ff);


#if 0
#ifdef USE_FOPEN
	char tmp;
	FILE *ff = fopen("/dev/uart","r");
	for (;;) {
		fread(&tmp , 1 , 1 , ff);
		printk("readed %c -- %d \n", tmp , tmp);
		fwrite(&tmp , 1 , 1 , ff);
	}
#endif

#ifdef USE_READLINE
	for (;;) {
		cmd_ptr = __readline(promt_ptr);
		shell_command_descriptor_find_first(cmd_ptr, -1);
		#if 1
			printk("CmdLine: %s, RunCommand: %s\n",cmd_ptr, scd->name);
		#endif
		ret_code = shell_command_exec(scd, 0, NULL);
	}
#endif
#endif

	return 0;
}

static int esh_stop(void) {
	return 0;
}

EMBOX_UNIT(esh_start, esh_stop);

/* tmp */

