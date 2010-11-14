/**
 * @file
 * @brief write memory statistic for mpallocator
 *
 * @date 14.11.10
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 */

#include <shell_command.h>
#include <kernel/mm/mpallocator.h>
#include <kernel/mm/kmalloc.h>
#include <lib/list.h>

#define COMMAND_NAME     "meminfo"
#define COMMAND_DESC_MSG "writes memory statistic for mpallocator"
#define HELP_MSG         "Usage: meminfo [-h] -s"
static const char *man_page =
#include "meminfo_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static LIST_HEAD(mpblocks_info_list);

static void mpget_mem_blocks() {
	struct list_head* cur_elem;
	block_info_t* cur_block;
	int free_blocks_count = 0;
	int free_bytes_count = 0, busy_bytes_count = 0;
	mpget_blocks_info(&mpblocks_info_list);

	printf("N  free/busy size\n");
	printf("-----------------\n");

	int i = 1;
	list_for_each(cur_elem, &mpblocks_info_list) {
		cur_block = (block_info_t*) cur_elem;
		free_blocks_count += (cur_block->free ? 1 : 0);
		free_bytes_count += (cur_block->free ? cur_block->size
				* CONFIG_PAGE_SIZE : 0);
		busy_bytes_count += (cur_block->free ? 0 : cur_block->size
				* CONFIG_PAGE_SIZE);

		if (cur_block->free)
			printf("%d. free      %d\n", i, cur_block->size);
		else
			printf("%d. busy      %d\n", i, cur_block->size);
		i++;
	}
	printf("-----------------\n");

	printf("Count of free memory blocks: %d\n", free_blocks_count);
	printf("Count of free memory bytes : %d\n", free_bytes_count);
	printf("Count of busy memory bytes : %d\n", busy_bytes_count);

	for (cur_elem = (&mpblocks_info_list)->next; cur_elem
			!= (&mpblocks_info_list);) {
		__list_del(cur_elem->prev, cur_elem->next);
		cur_block = (block_info_t*) cur_elem;
		cur_elem = cur_elem->next;
		free(cur_block);
	}
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hs");
		switch (nextOption) {
		case 'h':
			show_help();
			return 0;
		case 's':
			mpget_mem_blocks();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	return 0;
}
