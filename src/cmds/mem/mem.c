/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.2009
 * @author Alexey Fomin
 */
#include <shell_command.h>

#define COMMAND_NAME     "mem"
#define COMMAND_DESC_MSG "read from memory"
#define HELP_MSG         "Usage: mem [-h] [-a addr] [-n num]"
static const char *man_page =
	#include "mem_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void mem_print(uint32_t *addr, unsigned int amount) {
	long i = 0;
	uint32_t *cur_addr = (uint32_t *) ((uint32_t) addr & 0xFFFFFFFC);
	while (i < amount) {
		if (0 == (i % 4)) {
			printf("0x%08x:\t", (unsigned)cur_addr);
		}
		printf("0x%08x\t", *(unsigned *) cur_addr);
		if (3 == (i % 4)) {
			printf("\n");
		}
		cur_addr++;
		i++;
	}
}

//typedef void TEST_MEM_FUNC(uint32_t *addr, unsigned int amount);

static int exec(int argsc, char **argsv) {
	uint32_t *address = (uint32_t *) 0x70000000L;
	unsigned int amount = 100L;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "a:hn:");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'a':
			if ((optarg != NULL) &&
					(!sscanf(optarg, "0x%x", (unsigned *)(void *)&address)) &&
					(!sscanf(optarg, "%d", (int *)(void *)(&address)))) {
				LOG_ERROR("mem: -a: hex value for address expected.\n");
				show_help();
				return -1;
			}
			break;
		case 'n':
			if (optarg == NULL) {
				LOG_ERROR("mem: -n: hex or integer value for number of bytes expected.\n");
				show_help();
				return -1;
			}
			if (!sscanf(optarg, "0x%x", &amount)) {
				if (!sscanf(optarg, "%x", &amount)) {
					LOG_ERROR("mem: -n: hex or integer value for number of bytes expected.\n");
					show_help();
					return -1;
				}
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	mem_print(address, amount);
	return 0;
}
