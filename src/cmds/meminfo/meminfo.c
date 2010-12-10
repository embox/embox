/**
 * @file
 * @brief write memory statistic for mpallocator or kmalloc
 *
 * @date 14.11.10
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 * @author Dmitry Zubarevich
 */

#include <shell_command.h>
#include <kernel/mm/mpallocator.h>
#include <kernel/mm/kmalloc.h>
#include <lib/list.h>
#include <stdlib.h>

#define COMMAND_NAME     "meminfo"
#define COMMAND_DESC_MSG "write memory statistic for kmalloc or mpallocator"
#define HELP_MSG         "Usage: meminfo [-h] [-k] -m\n"

static const char *man_page =
#include "meminfo_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec,
	COMMAND_DESC_MSG, HELP_MSG, man_page);

static LIST_HEAD(mpblocks_info_list);
static LIST_HEAD(kmblocks_info_list);

/**
 * write memory statistic
 *
 * @param list of free and busy blocks
 */
static void print_statistic(struct list_head* list) {
	int i = 1;
	struct list_head* cur_elem;
	block_info_t* cur_block;
	int free_blocks_count = 0;
	int free_bytes_count = 0, busy_bytes_count = 0;

	printf("N  free/busy size\n");
	printf("-----------------\n");

	list_for_each(cur_elem, list) {
		cur_block = (block_info_t*) cur_elem;
		free_blocks_count += (cur_block->free ? 1 : 0);
		free_bytes_count += (cur_block->free ? cur_block->size
				* CONFIG_PAGE_SIZE : 0);
		busy_bytes_count += (cur_block->free ? 0 : cur_block->size
				* CONFIG_PAGE_SIZE);

		if (cur_block->free) {
			printf("%d. free      %d\n", i, cur_block->size);
		} else {
			printf("%d. busy      %d\n", i, cur_block->size);
		}
		i++;
	}
	printf("-----------------\n");

	printf("Count of free memory blocks: %d\n", free_blocks_count);
	printf("Count of free memory bytes : %d\n", free_bytes_count);
	printf("Count of busy memory bytes : %d\n", busy_bytes_count);
}

static void delete_list(struct list_head* list) {
	struct list_head* cur_elem;
	block_info_t* cur_block;

	for (cur_elem = list->next; cur_elem != list;) {
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
		nextOption = getopt(argsc, argsv, "hkm");
		switch (nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'm':
			mpget_blocks_info(&mpblocks_info_list);
			print_statistic(&mpblocks_info_list);
			delete_list(&mpblocks_info_list);
			return 0;
		case 'k':
			kmget_blocks_info(&kmblocks_info_list);
			print_statistic(&kmblocks_info_list);
			delete_list(&kmblocks_info_list);
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	return 0;
}

