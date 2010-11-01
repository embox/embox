/**
 * @file
 * @brief Execute image file.
 *
 * @date 02.07.2009
 * @author Sergey Kuzmin
 */
#include <shell_command.h>
#include <hal/interrupt.h>
#include <hal/ipl.h>

#define COMMAND_NAME     "goto"
#define COMMAND_DESC_MSG "execute image file"
#define HELP_MSG         "Usage: goto [-h] [-a addr]"
static const char *man_page =
	#include "goto_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

typedef void (*IMAGE_ENTRY)(void);

static void go_to(uint32_t addr) {
	interrupt_nr_t intr_nr;
	printf("Try goto 0x%08X\n", addr);
	ipl_disable();
	for (intr_nr = 0; intr_nr < INTERRUPT_NRS_TOTAL; ++intr_nr) {
		interrupt_disable(intr_nr);
	}
	((IMAGE_ENTRY)addr)();
}

static int exec(int argsc, char **argsv){
	int nextOption;
	uint32_t addr = 0;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "a:h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'a':
			if ((optarg == NULL) || (*optarg == '\0')) {
				LOG_ERROR("goto: missed address value\n");
				return -1;
			}
			if (sscanf(optarg, "0x%x", &addr) > 0) {
				go_to(addr);
				return 0;
			}
			LOG_ERROR("goto: invalid value \"%s\".", optarg);
			return -1;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	return 0;
}
