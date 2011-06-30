/**
 * @file
 * @brief write memory statistic for mpallocator, kmalloc, slab allocator
 *
 * @date 14.11.10
 * @author Kirill Tyushev
 * @author Alexandr Kalmuk
 * @author Dmitry Zubarevich
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <kernel/mm/pagealloc/mpallocator.h>
#include <kernel/mm/misc/slab.h>
#include <kernel/mm/slab_statistic.h>
#include <lib/list.h>
#include <stdlib.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: meminfo [-h] [-k] [-s] -m\n");
}

static LIST_HEAD(mpblocks_info_list);
static LIST_HEAD(kmblocks_info_list);
static LIST_HEAD(sblocks_info_list);
static LIST_HEAD(all_caches_list);

/**
 * Write memory statistic for mpallocator or kmalloc
 * @param list of free and busy blocks
 */
static void print_kmstatistic(struct list_head* list) {
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

static void print_stat_for_slab(struct list_head* list) {
	int i = 1;
	int j = 0;
	struct list_head* cur_elem;
	block_info_t* cur_block;
	int free_blocks_count = 0;

	printf("N     free/busy   size\n");
	printf("----------------------\n");

	cur_elem = list->next;
	do {
		cur_block = (block_info_t*) cur_elem;

		if (cur_block->free) {
			while (((block_info_t*) cur_elem)->free && cur_elem != list) {
				j++;
				free_blocks_count++;
				cur_elem = cur_elem->next;
			}
			if (i == i + j - 1) {
				printf("%d.      free      %d\n", i, cur_block->size);
			} else
				printf("%d - %d. free      %d\n", i, i + j - 1, cur_block->size);
		} else {
			while (!((block_info_t*) cur_elem)->free && cur_elem != list) {
				j++;
				cur_elem = cur_elem->next;
			}
			if (i == i + j - 1) {
				printf("%d.      busy      %d\n", i, cur_block->size);
			} else
				printf("%d - %d. busy      %d\n", i, i + j - 1, cur_block->size);
		}
		i = i + j;
		j = 0;

	} while (cur_elem != list);
	printf("Count of free memory blocks: %d\n", free_blocks_count);
	printf("----------------------\n\n");
}

static void print_sstatistic(struct list_head* pseudo_list) {
	struct list_head* cur_elem_cache;
	struct list_head* cur_elem_slab;
	cache_t *cur_cachep;

	make_caches_list(pseudo_list);

	cur_elem_cache = pseudo_list;
	/* analyze caches descriptors */
	do {
		cur_elem_cache = cur_elem_cache->next;
		cur_cachep = (cache_t*) cur_elem_cache;
		printf("name of cache: %s\n", cur_cachep->name);
		/* print statistic for each slab in appropriate slab list of cur_cache */
		printf("free slabs:\n");
		cur_elem_slab = cur_cachep->slabs_free.next;
		while (1) {
			if (cur_elem_slab == &(cur_cachep->slabs_free))
				break;
			sget_blocks_info(&sblocks_info_list, cur_elem_slab);
			print_stat_for_slab(&sblocks_info_list);
			delete_list(&sblocks_info_list);
			cur_elem_slab = cur_elem_slab->next;
		}
		printf("---------------------\n");

		printf("full slabs:\n");
		cur_elem_slab = cur_cachep->slabs_full.next;
		while (1) {
			if (cur_elem_slab == &(cur_cachep->slabs_full))
				break;
			sget_blocks_info(&sblocks_info_list, cur_elem_slab);
			print_stat_for_slab(&sblocks_info_list);
			delete_list(&sblocks_info_list);
			cur_elem_slab = cur_elem_slab->next;
		}
		printf("---------------------\n");

		printf("partial slabs:\n");
		cur_elem_slab = cur_cachep->slabs_partial.next;
		while (1) {
			if (cur_elem_slab == &(cur_cachep->slabs_partial))
				break;
			sget_blocks_info(&sblocks_info_list, cur_elem_slab);
			print_stat_for_slab(&sblocks_info_list);
			delete_list(&sblocks_info_list);
			cur_elem_slab = cur_elem_slab->next;
		}
		printf("---------------------\n");

		printf("++++++++++++++++++++++++++++++++\n\n");
		/* if current cache is the last cache in the all_caches_list */
	} while (cur_elem_cache != pseudo_list->prev);
}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	do {
		opt = getopt(argc, argv, "hkms");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'm':
			mpget_blocks_info(&mpblocks_info_list);
			print_kmstatistic(&mpblocks_info_list);
			delete_list(&mpblocks_info_list);
			return 0;
		case 'k':
			//kmget_blocks_info(&kmblocks_info_list);
			//print_kmstatistic(&kmblocks_info_list);
			//delete_list(&kmblocks_info_list);
			return 0;
		case 's':
			print_sstatistic(&all_caches_list);
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	return 0;
}
