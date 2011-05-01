/**
 * @file
 * @brief writes to memory
 *
 * @date 20.02.2009
 * @author Alexey Fomin
 */
#include <shell_command.h>

#define COMMAND_NAME     "wmem"
#define COMMAND_DESC_MSG "writes to memory"
#define HELP_MSG         "Usage: wmem [-h] -a addr -v value"
static const char *man_page =
	#include "wmem_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int flag = 0;
	int nextOption;
	uint32_t *address;
	uint32_t new_value;

	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "a:v:h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'a':
			if ((optarg != NULL) &&
					(!sscanf(optarg, "0x%x", (unsigned *)(void *)(&address)))
					&& (!sscanf(optarg, "%d", (int *)(void *)(&address)))) {
				LOG_ERROR("wmem: hex value expected.\n");
				show_help();
				return -1;
			}
			if ((unsigned)address != ((unsigned)address & 0xFFFFFFFC)) {
				LOG_ERROR("wmem: address in wrong format (last 2 bits must be 0)\n");
				show_help();
				return -1;
			}
			flag++;
			break;
		case 'v':
			if ((optarg != NULL) && (!sscanf(optarg, "0x%x", &new_value))
					&& (!sscanf(optarg, "%d", (int *) &new_value))) {
				LOG_ERROR("wmem: hex value expected.\n");
				show_help();
				return -1;
			}
			flag++;
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (flag != 2) {
		LOG_ERROR("wmem: -a and -v keys expected!\n");
		return -1;
	}

	*((uint32_t *) address) = new_value;
	return 0;
}
