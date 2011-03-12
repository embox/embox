/**
 * @file
 *
 * @date Jul 24, 2009
 * @author Alexey Fomin
 *         Daria Teplykh
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <types.h>
#include <asm/cache.h>
#include <string.h>

#include "memory_tests.h"

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: testmem [-h] [-a addr] [-n num] [-t type]\n");
}

/*
 *  interface for 'testmem' user command
 *
 */

static memtest_err_t last_err;

static int exec(int argsc, char **argsv) {
	TEST_MEM_FUNC *test_mem_func = NULL;
	uint32_t *address = (uint32_t *) 0x70000000L;
	long unsigned int amount = 1000000L;
	uint32_t template = (uint32_t) 0x55555555;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "a:n:t:h");
		switch (nextOption) {
		case 'h':
			show_man_page();
			return 0;
		case 'a':
			/* Key -a for address
			 Can be in hex and in decimal*/
			if ((optarg == NULL) || /* addr empty*/
			((!sscanf(optarg, "0x%x", (unsigned *) (void *) &address)) /* addr not in hex*/
			&& (!sscanf(optarg, "%d", (int *) (void *) &address)))) { /* addr not in decimal*/
				LOG_ERROR(
						"testmem: -a: address value in hex or in decimal expected.\n");
				print_usage();
				return -1;
			}
			break;
		case 'n':
			/* Key -n for number of
			 - times, starting loop in "loop" test
			 - bytes testing in other case
			 The same as address can be either in hex or decimal*/
			if (optarg == NULL) {
				LOG_ERROR(
						"testmem: -n: amount value in hex or in decimal expected.\n");
				print_usage();
				return -1;
			}
			if (!sscanf(optarg, "0x%lx", &amount)) {
				if (!sscanf(optarg, "%d", (int *) (void *) &amount)) {
					LOG_ERROR(
							"testmem: -n: amount value in hex or in decimal expected.\n");
					print_usage();
					return -1;
				}
			}
			break;
		case 't':
			/* Key -t for test type
			 If not presented - setting default type
			 !! for "loop" different default value
			 !!   for amount (in this case amount is
			 !!   a counter of loop)*/
			if (NULL == (test_mem_func = get_memtest_func(optarg))) {
				LOG_ERROR("testmem: %s: no such test.\n", optarg);
				print_usage();
				return -1;
			}

			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (test_mem_func == NULL) {
		LOG_ERROR("testmem: test name expected.\n");
		print_usage();
		return -1;
	}

	cache_data_disable();
	TRACE("Before starting: address: 0x%08x, amount: 0x%08x\n", (unsigned)address, (unsigned)amount);
	(*test_mem_func)(address, amount, template, &last_err);
	cache_data_enable();
	return 0;
}
