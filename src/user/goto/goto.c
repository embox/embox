/**
 * \file goto.c
 *
 * \date 02.07.2009
 * \author kse
 */

#include "asm/types.h"
#include "conio/conio.h"
#include "shell.h"
#include "shell_command.h"

#include "goto.h"

static char goto_keys[] = {
#include "goto_keys.inc"
		};

static void show_help() {
	printf(
#include "goto_help.inc"
	);
}
#define COMMAND_NAME "exec"
#define COMMAND_DESC_MSG "execute image file"
static const char *help_msg =
	#include "goto_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);

typedef void (*IMAGE_ENTRY)();

void go_to(unsigned int addr)
{
	printf("Try goto 0x%08X\n", addr);
	/*__asm__ __volatile__("jmpl %0, %%g0\nnop\n"::"rI"(addr));
    */
	timers_off();
	irq_ctrl_disable_all();
	((IMAGE_ENTRY)addr)();
}


//int goto_shell_handler(int argsc, char **argsv) {
static int exec(int argsc, char **argsv){
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	unsigned int addr = 0;

	keys_amount = parse_arg("goto", argsc, argsv, goto_keys, sizeof(goto_keys), keys);

	if (keys_amount < 0) {
		printf("no keys.\n");
		return -1;
	}


	if (get_key('a', keys, keys_amount, &key_value))
	{
		if ((key_value == NULL) || (*key_value == '\0'))
		{
			LOG_ERROR("goto: missed address value\n");
			return -1;
		}

		if (sscanf(key_value, "0x%x", &addr) > 0)
		{
			go_to(addr);
			return 0;
		}

		LOG_ERROR("goto: invalid value \"%s\".\n\the number expected.\n", key_value);
		return -1;
	}


	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	show_help();
	return -1;
}
